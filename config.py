import os
from pathlib import Path


BASE_DIR = Path(__file__).resolve().parent


class Config:
    SECRET_KEY = os.getenv("SECRET_KEY", "dev-secret-key")
    COMPANY_NAME = "G-Son Allservice AB"
    SERVICE_AREA = "Uddevalla med omnejd"
    DEFAULT_EMAIL = os.getenv("CONTACT_EMAIL", "info@gsonallservice.se")
    ORG_NUMBER = os.getenv("ORG_NUMBER", "559000-0000")
    SUBMISSIONS_DIR = BASE_DIR / "instance"
    CONTACT_SUBMISSIONS_FILE = SUBMISSIONS_DIR / "contact_submissions.csv"
    QUOTE_SUBMISSIONS_FILE = SUBMISSIONS_DIR / "quote_submissions.csv"
