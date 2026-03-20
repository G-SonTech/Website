from datetime import datetime


def current_year() -> int:
    return datetime.now().year


def stars(count: int) -> str:
    return "★" * count
