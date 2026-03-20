from __future__ import annotations

import os
import shutil
import sys
from pathlib import Path


ROOT_DIR = Path(__file__).resolve().parent.parent
DIST_DIR = ROOT_DIR / "dist"
STATIC_DIR = ROOT_DIR / "static"
sys.path.insert(0, str(ROOT_DIR))


def _prepare_environment() -> None:
    os.environ["STATIC_EXPORT"] = "1"
    if os.getenv("URL") and not os.getenv("SITE_URL"):
        os.environ["SITE_URL"] = os.getenv("URL", "")


def _route_to_output_path(route: str) -> Path:
    clean_route = route.split("?", 1)[0]
    if clean_route == "/":
        return DIST_DIR / "index.html"
    return DIST_DIR / clean_route.lstrip("/") / "index.html"


def _copy_static_assets() -> None:
    shutil.copytree(STATIC_DIR, DIST_DIR / "static")


def _render_pages() -> None:
    from app import app
    from data.services import SERVICES

    routes = [
        "/",
        "/om-oss",
        "/tjanster",
        "/referenser",
        "/recensioner",
        "/offert",
        "/kontakt",
        "/faq",
        "/integritet",
        "/tack",
    ]
    routes.extend(f"/tjanster/{service['slug']}" for service in SERVICES)

    with app.test_client() as client:
        for route in routes:
            response = client.get(route)
            if response.status_code != 200:
                raise RuntimeError(f"Failed to render {route}: HTTP {response.status_code}")
            output_path = _route_to_output_path(route)
            output_path.parent.mkdir(parents=True, exist_ok=True)
            output_path.write_bytes(response.get_data())

        not_found = client.get("/den-har-sidan-finns-inte")
        (DIST_DIR / "404.html").write_bytes(not_found.get_data())


def _write_redirects() -> None:
    # Let Netlify serve generated files and fall back to static 404 page.
    (DIST_DIR / "_redirects").write_text("/* /404.html 404\n", encoding="utf-8")


def main() -> None:
    _prepare_environment()
    shutil.rmtree(DIST_DIR, ignore_errors=True)
    DIST_DIR.mkdir(parents=True, exist_ok=True)
    _copy_static_assets()
    _render_pages()
    _write_redirects()
    print(f"Static site generated in {DIST_DIR}")


if __name__ == "__main__":
    main()
