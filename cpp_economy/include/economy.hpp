#pragma once

#include <filesystem>
#include <map>
#include <ostream>
#include <string>
#include <vector>

namespace gson::economy {

struct CompanyInfo {
    std::string company_name;
    std::string org_number;
    std::string email;
};

struct Account {
    std::string number;
    std::string name;
    std::string type;
};

struct VoucherLine {
    int voucher_id = 0;
    std::string account;
    std::string text;
    double debit = 0.0;
    double credit = 0.0;
};

struct Voucher {
    int id = 0;
    std::string date;
    std::string description;
    std::vector<VoucherLine> lines;
};

struct InvoiceLine {
    std::string description;
    double quantity = 0.0;
    double unit_price = 0.0;
    double vat_rate = 0.25;

    double net_amount() const;
    double vat_amount() const;
    double gross_amount() const;
};

struct Invoice {
    int id = 0;
    std::string customer;
    std::string issue_date;
    std::string due_date;
    std::string description;
    bool paid = false;
    std::vector<InvoiceLine> lines;

    double net_total() const;
    double vat_total() const;
    double gross_total() const;
};

struct VatReport {
    std::string period;
    double output_vat = 0.0;
    double input_vat = 0.0;
    double payable_vat = 0.0;
};

class Ledger {
public:
    explicit Ledger(std::filesystem::path project_root);

    void initialize_default_accounts();
    void load();
    void save() const;

    int issue_service_invoice(
        const std::string& issue_date,
        const std::string& due_date,
        const std::string& customer,
        const std::vector<InvoiceLine>& lines,
        const std::string& description
    );

    void register_invoice_payment(
        int invoice_id,
        const std::string& payment_date,
        const std::string& bank_account = "1930"
    );

    std::map<std::string, double> account_balances() const;
    VatReport vat_report(const std::string& year_month) const;

    void print_trial_balance(std::ostream& os) const;

    const CompanyInfo& company_info() const;
    const std::vector<Invoice>& invoices() const;
    const std::vector<Voucher>& vouchers() const;

private:
    std::filesystem::path project_root_;
    std::filesystem::path data_dir_;
    CompanyInfo company_info_;
    std::map<std::string, Account> accounts_;
    std::vector<Voucher> vouchers_;
    std::vector<Invoice> invoices_;
    int next_voucher_id_ = 1;
    int next_invoice_id_ = 1;

    static double round2(double value);
    static std::string to_money(double value);

    int create_voucher(
        const std::string& date,
        const std::string& description,
        const std::vector<VoucherLine>& lines
    );

    Invoice* find_invoice(int id);
    const Invoice* find_invoice(int id) const;

    static CompanyInfo load_company_info(const std::filesystem::path& project_root);
};

} // namespace gson::economy
