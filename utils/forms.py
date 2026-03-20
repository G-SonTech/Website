import re
from html import escape


CONTACT_REASONS = ["Fråga", "Offertförfrågan", "Boka arbete"]
CUSTOMER_TYPES = ["Privatperson", "Företag", "BRF / Fastighet"]
EMAIL_PATTERN = re.compile(r"^[^@\s]+@[^@\s]+\.[^@\s]+$")

CONTACT_DEFAULTS = {
    "name": "",
    "phone": "",
    "email": "",
    "subject": "",
    "reason": CONTACT_REASONS[0],
    "message": "",
    "website": "",
}

QUOTE_DEFAULTS = {
    "name": "",
    "company": "",
    "phone": "",
    "email": "",
    "location": "",
    "service_type": "",
    "timeline": "",
    "customer_type": CUSTOMER_TYPES[0],
    "description": "",
    "privacy_consent": "",
    "website": "",
}


def _clean(source, key: str, default: str = "") -> str:
    return escape(source.get(key, default).strip())


def get_contact_form_data(source) -> dict[str, str]:
    return {
        "name": _clean(source, "name"),
        "phone": _clean(source, "phone"),
        "email": _clean(source, "email"),
        "subject": _clean(source, "subject"),
        "reason": _clean(source, "reason", CONTACT_REASONS[0]),
        "message": _clean(source, "message"),
        "website": source.get("website", "").strip(),
    }


def get_quote_form_data(source) -> dict[str, str]:
    return {
        "name": _clean(source, "name"),
        "company": _clean(source, "company"),
        "phone": _clean(source, "phone"),
        "email": _clean(source, "email"),
        "location": _clean(source, "location"),
        "service_type": _clean(source, "service_type"),
        "timeline": _clean(source, "timeline"),
        "customer_type": _clean(source, "customer_type", CUSTOMER_TYPES[0]),
        "description": _clean(source, "description"),
        "privacy_consent": source.get("privacy_consent", "").strip(),
        "website": source.get("website", "").strip(),
    }


def _validate_email(email: str) -> bool:
    return bool(EMAIL_PATTERN.match(email))


def _validate_phone(phone: str) -> bool:
    return len(re.sub(r"\D", "", phone)) >= 7


def validate_contact_form(data: dict[str, str]) -> dict[str, str]:
    errors: dict[str, str] = {}
    if data["website"]:
        errors["form"] = "Formuläret kunde inte skickas. Försök igen."
    if len(data["name"]) < 2:
        errors["name"] = "Ange ditt namn."
    if not _validate_phone(data["phone"]):
        errors["phone"] = "Ange ett giltigt telefonnummer."
    if not _validate_email(data["email"]):
        errors["email"] = "Ange en giltig e-postadress."
    if len(data["subject"]) < 3:
        errors["subject"] = "Ange ett tydligt ämne."
    if data["reason"] not in CONTACT_REASONS:
        errors["reason"] = "Välj en giltig ärendetyp."
    if len(data["message"]) < 10:
        errors["message"] = "Beskriv ditt ärende med minst 10 tecken."
    return errors


def validate_quote_form(data: dict[str, str], service_options: list[str]) -> dict[str, str]:
    errors: dict[str, str] = {}
    if data["website"]:
        errors["form"] = "Formuläret kunde inte skickas. Försök igen."
    if len(data["name"]) < 2:
        errors["name"] = "Ange ditt namn."
    if not _validate_phone(data["phone"]):
        errors["phone"] = "Ange ett giltigt telefonnummer."
    if not _validate_email(data["email"]):
        errors["email"] = "Ange en giltig e-postadress."
    if len(data["location"]) < 2:
        errors["location"] = "Ange ort eller område."
    if data["service_type"] not in service_options:
        errors["service_type"] = "Välj en tjänst som passar uppdraget."
    if len(data["description"]) < 20:
        errors["description"] = "Beskriv uppdraget med minst 20 tecken."
    if len(data["timeline"]) < 2:
        errors["timeline"] = "Ange önskat datum eller tidsram."
    if data["customer_type"] not in CUSTOMER_TYPES:
        errors["customer_type"] = "Välj kundtyp."
    if data["privacy_consent"] != "accepted":
        errors["privacy_consent"] = "Du behöver godkänna integritetspolicyn."
    return errors
