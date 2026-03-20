import os
from pathlib import Path


BASE_DIR = Path(__file__).resolve().parent


def _resolve_submissions_dir() -> Path:
    custom_dir = os.getenv("SUBMISSIONS_DIR")
    if custom_dir:
        return Path(custom_dir)

    # Netlify/AWS Lambda filesystems are read-only except /tmp.
    if os.getenv("NETLIFY") or os.getenv("AWS_LAMBDA_FUNCTION_NAME") or os.getenv("LAMBDA_TASK_ROOT"):
        return Path("/tmp/gson-submissions")

    return BASE_DIR / "instance"


class Config:
    SECRET_KEY = os.getenv("SECRET_KEY", "dev-secret-key")
    COMPANY_NAME = "G-Son Allservice AB"
    SERVICE_AREA = "Uddevalla med omnejd"
    DEFAULT_EMAIL = os.getenv("CONTACT_EMAIL", "info@gsonallservice.se")
    ORG_NUMBER = os.getenv("ORG_NUMBER", "559000-0000")
    SITE_URL = os.getenv("SITE_URL", "").rstrip("/")
    STATIC_EXPORT = os.getenv("STATIC_EXPORT", "0") == "1"
    SUBMISSIONS_DIR = _resolve_submissions_dir()
    CONTACT_SUBMISSIONS_FILE = SUBMISSIONS_DIR / "contact_submissions.csv"
    QUOTE_SUBMISSIONS_FILE = SUBMISSIONS_DIR / "quote_submissions.csv"
