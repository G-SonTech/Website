#include "economy.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <regex>
#include <sstream>
#include <stdexcept>

namespace {

using gson::economy::CompanyInfo;

constexpr double kEpsilon = 0.01;

std::string trim(const std::string& value) {
    const auto begin = value.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) {
        return "";
    }
    const auto end = value.find_last_not_of(" \t\r\n");
    return value.substr(begin, end - begin + 1);
}

bool starts_with(const std::string& text, const std::string& prefix) {
    return text.size() >= prefix.size() && text.compare(0, prefix.size(), prefix) == 0;
}

double round2_local(double value) {
    return std::round(value * 100.0) / 100.0;
}

std::string csv_escape(const std::string& value) {
    const bool needs_quotes = value.find(',') != std::string::npos || value.find('"') != std::string::npos ||
                              value.find('\n') != std::string::npos || value.find('\r') != std::string::npos;
    if (!needs_quotes) {
        return value;
    }
    std::string escaped;
    escaped.reserve(value.size() + 4);
    escaped.push_back('"');
    for (const char ch : value) {
        if (ch == '"') {
            escaped.push_back('"');
        }
        escaped.push_back(ch);
    }
    escaped.push_back('"');
    return escaped;
}

std::vector<std::string> parse_csv_row(const std::string& line) {
    std::vector<std::string> cells;
    std::string current;
    bool in_quotes = false;

    for (std::size_t i = 0; i < line.size(); ++i) {
        const char ch = line[i];
        if (ch == '"') {
            if (in_quotes && i + 1 < line.size() && line[i + 1] == '"') {
                current.push_back('"');
                ++i;
            } else {
                in_quotes = !in_quotes;
            }
        } else if (ch == ',' && !in_quotes) {
            cells.push_back(current);
            current.clear();
        } else {
            current.push_back(ch);
        }
    }

    cells.push_back(current);
    return cells;
}

void write_csv_row(std::ofstream& file, const std::vector<std::string>& row) {
    for (std::size_t i = 0; i < row.size(); ++i) {
        if (i > 0) {
            file << ',';
        }
        file << csv_escape(row[i]);
    }
    file << '\n';
}

int parse_int(const std::string& value) {
    return std::stoi(value);
}

double parse_double(const std::string& value) {
    if (value.empty()) {
        return 0.0;
    }
    return std::stod(value);
}

std::map<std::string, std::string> parse_env_file(const std::filesystem::path& path) {
    std::map<std::string, std::string> vars;
    std::ifstream file(path);
    if (!file.is_open()) {
        return vars;
    }

    std::string line;
    while (std::getline(file, line)) {
        const std::string cleaned = trim(line);
        if (cleaned.empty() || starts_with(cleaned, "#")) {
            continue;
        }

        const auto equal_pos = cleaned.find('=');
        if (equal_pos == std::string::npos) {
            continue;
        }

        std::string key = trim(cleaned.substr(0, equal_pos));
        std::string value = trim(cleaned.substr(equal_pos + 1));
        if (!value.empty() && (value.front() == '"' || value.front() == '\'')) {
            value.erase(value.begin());
        }
        if (!value.empty() && (value.back() == '"' || value.back() == '\'')) {
            value.pop_back();
        }
        vars[key] = value;
    }

    return vars;
}

std::string find_python_string_value(
    const std::filesystem::path& config_path,
    const std::string& variable,
    const std::string& fallback
) {
    std::ifstream file(config_path);
    if (!file.is_open()) {
        return fallback;
    }

    const std::regex pattern("^\\s*" + variable + "\\s*=\\s*[\"']([^\"']*)[\"']");

    std::string line;
    std::smatch match;
    while (std::getline(file, line)) {
        if (std::regex_search(line, match, pattern) && match.size() > 1) {
            return match[1].str();
        }
    }

    return fallback;
}

} // namespace

namespace gson::economy {

double InvoiceLine::net_amount() const {
    return round2_local(quantity * unit_price);
}

double InvoiceLine::vat_amount() const {
    return round2_local(net_amount() * vat_rate);
}

double InvoiceLine::gross_amount() const {
    return round2_local(net_amount() + vat_amount());
}

double Invoice::net_total() const {
    double total = 0.0;
    for (const auto& line : lines) {
        total += line.net_amount();
    }
    return round2_local(total);
}

double Invoice::vat_total() const {
    double total = 0.0;
    for (const auto& line : lines) {
        total += line.vat_amount();
    }
    return round2_local(total);
}

double Invoice::gross_total() const {
    return round2_local(net_total() + vat_total());
}

Ledger::Ledger(std::filesystem::path project_root)
    : project_root_(std::move(project_root)),
      data_dir_(project_root_ / "instance" / "economy"),
      company_info_(load_company_info(project_root_)) {}

void Ledger::initialize_default_accounts() {
    const std::vector<Account> defaults = {
        {"1510", "Kundfordringar", "asset"},
        {"1930", "Foretagskonto", "asset"},
        {"2440", "Leverantorsskulder", "liability"},
        {"2611", "Utgaende moms 25%", "liability"},
        {"2641", "Ingaende moms", "asset"},
        {"3041", "Forsaljning tjanster 25% moms", "revenue"},
        {"4010", "Inkop varor och material", "expense"},
        {"7010", "Lon till tjansteman", "expense"}
    };

    for (const auto& account : defaults) {
        accounts_[account.number] = account;
    }
}

void Ledger::load() {
    accounts_.clear();
    vouchers_.clear();
    invoices_.clear();
    next_voucher_id_ = 1;
    next_invoice_id_ = 1;

    const auto accounts_path = data_dir_ / "accounts.csv";
    const auto vouchers_path = data_dir_ / "vouchers.csv";
    const auto voucher_lines_path = data_dir_ / "voucher_lines.csv";
    const auto invoices_path = data_dir_ / "invoices.csv";
    const auto invoice_lines_path = data_dir_ / "invoice_lines.csv";

    if (std::ifstream file(accounts_path); file.is_open()) {
        std::string line;
        std::getline(file, line);
        while (std::getline(file, line)) {
            const auto row = parse_csv_row(line);
            if (row.size() < 3) {
                continue;
            }
            accounts_[row[0]] = Account{row[0], row[1], row[2]};
        }
    }

    std::map<int, Voucher> voucher_map;
    if (std::ifstream file(vouchers_path); file.is_open()) {
        std::string line;
        std::getline(file, line);
        while (std::getline(file, line)) {
            const auto row = parse_csv_row(line);
            if (row.size() < 3) {
                continue;
            }
            Voucher voucher;
            voucher.id = parse_int(row[0]);
            voucher.date = row[1];
            voucher.description = row[2];
            voucher_map[voucher.id] = voucher;
            next_voucher_id_ = std::max(next_voucher_id_, voucher.id + 1);
        }
    }

    if (std::ifstream file(voucher_lines_path); file.is_open()) {
        std::string line;
        std::getline(file, line);
        while (std::getline(file, line)) {
            const auto row = parse_csv_row(line);
            if (row.size() < 5) {
                continue;
            }
            const int voucher_id = parse_int(row[0]);
            auto it = voucher_map.find(voucher_id);
            if (it == voucher_map.end()) {
                continue;
            }
            it->second.lines.push_back(VoucherLine{
                voucher_id,
                row[1],
                row[2],
                parse_double(row[3]),
                parse_double(row[4]),
            });
        }
    }

    vouchers_.reserve(voucher_map.size());
    for (auto& [id, voucher] : voucher_map) {
        (void)id;
        vouchers_.push_back(std::move(voucher));
    }

    std::map<int, Invoice> invoice_map;
    if (std::ifstream file(invoices_path); file.is_open()) {
        std::string line;
        std::getline(file, line);
        while (std::getline(file, line)) {
            const auto row = parse_csv_row(line);
            if (row.size() < 6) {
                continue;
            }
            Invoice invoice;
            invoice.id = parse_int(row[0]);
            invoice.customer = row[1];
            invoice.issue_date = row[2];
            invoice.due_date = row[3];
            invoice.description = row[4];
            invoice.paid = row[5] == "1" || row[5] == "true";
            invoice_map[invoice.id] = invoice;
            next_invoice_id_ = std::max(next_invoice_id_, invoice.id + 1);
        }
    }

    if (std::ifstream file(invoice_lines_path); file.is_open()) {
        std::string line;
        std::getline(file, line);
        while (std::getline(file, line)) {
            const auto row = parse_csv_row(line);
            if (row.size() < 5) {
                continue;
            }
            const int invoice_id = parse_int(row[0]);
            auto it = invoice_map.find(invoice_id);
            if (it == invoice_map.end()) {
                continue;
            }
            it->second.lines.push_back(InvoiceLine{
                row[1],
                parse_double(row[2]),
                parse_double(row[3]),
                parse_double(row[4]),
            });
        }
    }

    invoices_.reserve(invoice_map.size());
    for (auto& [id, invoice] : invoice_map) {
        (void)id;
        invoices_.push_back(std::move(invoice));
    }

    if (accounts_.empty()) {
        initialize_default_accounts();
    }
}

void Ledger::save() const {
    std::filesystem::create_directories(data_dir_);

    const auto accounts_path = data_dir_ / "accounts.csv";
    const auto vouchers_path = data_dir_ / "vouchers.csv";
    const auto voucher_lines_path = data_dir_ / "voucher_lines.csv";
    const auto invoices_path = data_dir_ / "invoices.csv";
    const auto invoice_lines_path = data_dir_ / "invoice_lines.csv";

    {
        std::ofstream file(accounts_path, std::ios::trunc);
        if (!file.is_open()) {
            throw std::runtime_error("Kunde inte skriva accounts.csv");
        }
        write_csv_row(file, {"number", "name", "type"});
        for (const auto& [number, account] : accounts_) {
            (void)number;
            write_csv_row(file, {account.number, account.name, account.type});
        }
    }

    {
        std::ofstream file(vouchers_path, std::ios::trunc);
        if (!file.is_open()) {
            throw std::runtime_error("Kunde inte skriva vouchers.csv");
        }
        write_csv_row(file, {"id", "date", "description"});
        for (const auto& voucher : vouchers_) {
            write_csv_row(file, {
                std::to_string(voucher.id),
                voucher.date,
                voucher.description,
            });
        }
    }

    {
        std::ofstream file(voucher_lines_path, std::ios::trunc);
        if (!file.is_open()) {
            throw std::runtime_error("Kunde inte skriva voucher_lines.csv");
        }
        write_csv_row(file, {"voucher_id", "account", "text", "debit", "credit"});
        for (const auto& voucher : vouchers_) {
            for (const auto& line : voucher.lines) {
                write_csv_row(file, {
                    std::to_string(voucher.id),
                    line.account,
                    line.text,
                    to_money(line.debit),
                    to_money(line.credit),
                });
            }
        }
    }

    {
        std::ofstream file(invoices_path, std::ios::trunc);
        if (!file.is_open()) {
            throw std::runtime_error("Kunde inte skriva invoices.csv");
        }
        write_csv_row(file, {"id", "customer", "issue_date", "due_date", "description", "paid"});
        for (const auto& invoice : invoices_) {
            write_csv_row(file, {
                std::to_string(invoice.id),
                invoice.customer,
                invoice.issue_date,
                invoice.due_date,
                invoice.description,
                invoice.paid ? "1" : "0",
            });
        }
    }

    {
        std::ofstream file(invoice_lines_path, std::ios::trunc);
        if (!file.is_open()) {
            throw std::runtime_error("Kunde inte skriva invoice_lines.csv");
        }
        write_csv_row(file, {"invoice_id", "description", "quantity", "unit_price", "vat_rate"});
        for (const auto& invoice : invoices_) {
            for (const auto& line : invoice.lines) {
                write_csv_row(file, {
                    std::to_string(invoice.id),
                    line.description,
                    to_money(line.quantity),
                    to_money(line.unit_price),
                    to_money(line.vat_rate),
                });
            }
        }
    }
}

int Ledger::issue_service_invoice(
    const std::string& issue_date,
    const std::string& due_date,
    const std::string& customer,
    const std::vector<InvoiceLine>& lines,
    const std::string& description
) {
    if (lines.empty()) {
        throw std::runtime_error("En faktura maste ha minst en rad.");
    }

    for (const auto& line : lines) {
        if (line.quantity <= 0.0) {
            throw std::runtime_error("Fakturarad har ogiltigt antal.");
        }
        if (line.unit_price < 0.0 || line.vat_rate < 0.0) {
            throw std::runtime_error("Fakturarad har ogiltigt pris eller momssats.");
        }
    }

    Invoice invoice;
    invoice.id = next_invoice_id_++;
    invoice.customer = customer;
    invoice.issue_date = issue_date;
    invoice.due_date = due_date;
    invoice.description = description;
    invoice.paid = false;
    invoice.lines = lines;
    invoices_.push_back(invoice);

    std::vector<VoucherLine> voucher_lines;
    voucher_lines.push_back(VoucherLine{
        0,
        "1510",
        "Kundfordran faktura " + std::to_string(invoice.id),
        invoice.gross_total(),
        0.0,
    });

    for (const auto& line : lines) {
        const double net = line.net_amount();
        const double vat = line.vat_amount();

        if (net > 0.0) {
            voucher_lines.push_back(VoucherLine{
                0,
                "3041",
                "Forsaljning " + line.description,
                0.0,
                net,
            });
        }

        if (vat > 0.0) {
            voucher_lines.push_back(VoucherLine{
                0,
                "2611",
                "Utgaende moms " + line.description,
                0.0,
                vat,
            });
        }
    }

    create_voucher(
        issue_date,
        "Faktura " + std::to_string(invoice.id) + " - " + customer + " - " + description,
        voucher_lines
    );

    return invoice.id;
}

void Ledger::register_invoice_payment(int invoice_id, const std::string& payment_date, const std::string& bank_account) {
    Invoice* invoice = find_invoice(invoice_id);
    if (invoice == nullptr) {
        throw std::runtime_error("Faktura hittades inte.");
    }
    if (invoice->paid) {
        throw std::runtime_error("Fakturan ar redan markerad som betald.");
    }

    const double gross = invoice->gross_total();
    create_voucher(
        payment_date,
        "Inbetalning faktura " + std::to_string(invoice_id),
        {
            VoucherLine{0, bank_account, "Bankinbetalning", gross, 0.0},
            VoucherLine{0, "1510", "Reglering kundfordran", 0.0, gross},
        }
    );

    invoice->paid = true;
}

std::map<std::string, double> Ledger::account_balances() const {
    std::map<std::string, double> balances;
    for (const auto& [number, account] : accounts_) {
        (void)account;
        balances[number] = 0.0;
    }

    for (const auto& voucher : vouchers_) {
        for (const auto& line : voucher.lines) {
            balances[line.account] += round2(line.debit - line.credit);
        }
    }

    for (auto& [account, balance] : balances) {
        (void)account;
        balance = round2(balance);
    }
    return balances;
}

VatReport Ledger::vat_report(const std::string& year_month) const {
    if (year_month.size() != 7 || year_month[4] != '-') {
        throw std::runtime_error("Ange period som YYYY-MM.");
    }

    VatReport report;
    report.period = year_month;

    for (const auto& voucher : vouchers_) {
        if (!starts_with(voucher.date, year_month)) {
            continue;
        }
        for (const auto& line : voucher.lines) {
            if (starts_with(line.account, "261")) {
                report.output_vat += line.credit - line.debit;
            } else if (starts_with(line.account, "264")) {
                report.input_vat += line.debit - line.credit;
            }
        }
    }

    report.output_vat = round2(report.output_vat);
    report.input_vat = round2(report.input_vat);
    report.payable_vat = round2(report.output_vat - report.input_vat);
    return report;
}

void Ledger::print_trial_balance(std::ostream& os) const {
    const auto balances = account_balances();
    double total_debit = 0.0;
    double total_credit = 0.0;

    os << "Konto  Namn                           Debet       Kredit\n";
    os << "------------------------------------------------------------\n";

    for (const auto& [number, account] : accounts_) {
        const double balance = balances.contains(number) ? balances.at(number) : 0.0;
        const double debit = balance >= 0.0 ? balance : 0.0;
        const double credit = balance < 0.0 ? -balance : 0.0;
        total_debit += debit;
        total_credit += credit;

        os << std::left << std::setw(6) << number << " "
           << std::left << std::setw(30) << account.name
           << std::right << std::setw(11) << to_money(debit) << " "
           << std::right << std::setw(11) << to_money(credit) << '\n';
    }

    os << "------------------------------------------------------------\n";
    os << std::left << std::setw(37) << "Totalt"
       << std::right << std::setw(11) << to_money(total_debit) << " "
       << std::right << std::setw(11) << to_money(total_credit) << '\n';

    const double diff = round2(total_debit - total_credit);
    os << "Kontroll: " << (std::fabs(diff) <= kEpsilon ? "OK" : "OBALANS") << '\n';
}

const CompanyInfo& Ledger::company_info() const {
    return company_info_;
}

const std::vector<Invoice>& Ledger::invoices() const {
    return invoices_;
}

const std::vector<Voucher>& Ledger::vouchers() const {
    return vouchers_;
}

double Ledger::round2(double value) {
    return round2_local(value);
}

std::string Ledger::to_money(double value) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2) << round2(value);
    return ss.str();
}

int Ledger::create_voucher(const std::string& date, const std::string& description, const std::vector<VoucherLine>& lines) {
    if (lines.empty()) {
        throw std::runtime_error("Verifikat maste ha minst en rad.");
    }

    double debit = 0.0;
    double credit = 0.0;
    for (const auto& line : lines) {
        if (!accounts_.contains(line.account)) {
            throw std::runtime_error("Saknat konto i kontoplan: " + line.account);
        }
        debit += line.debit;
        credit += line.credit;
    }

    debit = round2(debit);
    credit = round2(credit);
    if (std::fabs(debit - credit) > kEpsilon) {
        throw std::runtime_error("Obalanserat verifikat. Debet=" + to_money(debit) + ", Kredit=" + to_money(credit));
    }

    Voucher voucher;
    voucher.id = next_voucher_id_++;
    voucher.date = date;
    voucher.description = description;
    voucher.lines.reserve(lines.size());

    for (const auto& line : lines) {
        VoucherLine copy = line;
        copy.voucher_id = voucher.id;
        copy.debit = round2(copy.debit);
        copy.credit = round2(copy.credit);
        voucher.lines.push_back(std::move(copy));
    }

    vouchers_.push_back(std::move(voucher));
    return vouchers_.back().id;
}

Invoice* Ledger::find_invoice(int id) {
    auto it = std::find_if(invoices_.begin(), invoices_.end(), [id](const Invoice& invoice) {
        return invoice.id == id;
    });
    return it == invoices_.end() ? nullptr : &(*it);
}

const Invoice* Ledger::find_invoice(int id) const {
    auto it = std::find_if(invoices_.begin(), invoices_.end(), [id](const Invoice& invoice) {
        return invoice.id == id;
    });
    return it == invoices_.end() ? nullptr : &(*it);
}

CompanyInfo Ledger::load_company_info(const std::filesystem::path& project_root) {
    const auto config_path = project_root / "config.py";
    CompanyInfo info;
    info.company_name = find_python_string_value(config_path, "COMPANY_NAME", "G-Son Allservice AB");
    info.org_number = find_python_string_value(config_path, "ORG_NUMBER", "559000-0000");
    info.email = find_python_string_value(config_path, "DEFAULT_EMAIL", "info@gsonallservice.se");

    const auto env_values = parse_env_file(project_root / ".env");
    if (auto it = env_values.find("CONTACT_EMAIL"); it != env_values.end() && !it->second.empty()) {
        info.email = it->second;
    }
    if (auto it = env_values.find("ORG_NUMBER"); it != env_values.end() && !it->second.empty()) {
        info.org_number = it->second;
    }
    if (auto it = env_values.find("COMPANY_NAME"); it != env_values.end() && !it->second.empty()) {
        info.company_name = it->second;
    }

    return info;
}

} // namespace gson::economy
