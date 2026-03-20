from csv import DictWriter
from pathlib import Path

from dotenv import load_dotenv
from flask import Flask, flash, redirect, render_template, request, url_for

from config import BASE_DIR, Config
from data.faq import FAQ_ITEMS
from data.locations import LOCATIONS
from data.references import REFERENCES
from data.reviews import REVIEWS
from data.services import SERVICES
from utils.forms import (
    CONTACT_DEFAULTS,
    CONTACT_REASONS,
    CUSTOMER_TYPES,
    QUOTE_DEFAULTS,
    get_contact_form_data,
    get_quote_form_data,
    validate_contact_form,
    validate_quote_form,
)
from utils.helpers import current_year, stars


ENV_PATH = BASE_DIR / ".env"
if ENV_PATH.exists():
    load_dotenv(ENV_PATH)


def create_app() -> Flask:
    app = Flask(__name__, instance_relative_config=True)
    app.config.from_object(Config)
    Path(app.config["SUBMISSIONS_DIR"]).mkdir(parents=True, exist_ok=True)

    service_lookup = {service["slug"]: service for service in SERVICES}
    contacts = [
        {
            "name": "Sebastian Göransson",
            "phone": "070-123 45 67",
            "email": "sebastian@gsonallservice.se",
        },
        {
            "name": "Daniel Göransson",
            "phone": "070-234 56 78",
            "email": "daniel@gsonallservice.se",
        },
    ]

    @app.context_processor
    def inject_site_data():
        return {
            "company_name": app.config["COMPANY_NAME"],
            "service_area": app.config["SERVICE_AREA"],
            "contacts": contacts,
            "default_email": app.config["DEFAULT_EMAIL"],
            "contact_reasons": CONTACT_REASONS,
            "customer_types": CUSTOMER_TYPES,
            "locations": LOCATIONS,
            "current_year": current_year(),
            "org_number": app.config["ORG_NUMBER"],
            "request_path": request.path,
        }

    @app.template_filter("stars")
    def stars_filter(value: int) -> str:
        return stars(value)

    @app.get("/")
    def index():
        return render_template(
            "index.html",
            services=SERVICES[:6],
            references=REFERENCES[:3],
            reviews=REVIEWS[:3],
            faq_items=FAQ_ITEMS[:3],
        )

    @app.get("/om-oss")
    def about():
        return render_template("about.html")

    @app.get("/tjanster")
    def services_page():
        return render_template("services.html", services=SERVICES)

    @app.get("/tjanster/<slug>")
    def service_detail(slug: str):
        service = service_lookup.get(slug)
        if service is None:
            return render_template("404.html"), 404
        return render_template("service_detail.html", service=service)

    @app.get("/referenser")
    def references_page():
        return render_template("references.html", references=REFERENCES)

    @app.get("/recensioner")
    def reviews_page():
        return render_template("reviews.html", reviews=REVIEWS)

    @app.route("/offert", methods=["GET", "POST"])
    def quote():
        service_titles = [service["title"] for service in SERVICES]
        requested_service = request.args.get("tjanst", "").strip()
        form_data = {**QUOTE_DEFAULTS}
        if requested_service in service_titles:
            form_data["service_type"] = requested_service
        errors = {}

        if request.method == "POST":
            form_data = get_quote_form_data(request.form)
            errors = validate_quote_form(form_data, service_titles)
            if not errors:
                save_submission(app.config["QUOTE_SUBMISSIONS_FILE"], form_data, [
                    "name",
                    "company",
                    "phone",
                    "email",
                    "location",
                    "service_type",
                    "timeline",
                    "customer_type",
                    "description",
                ])
                flash("Tack! Din offertförfrågan är skickad. Vi återkommer så snart vi kan.", "success")
                return redirect(url_for("thank_you", form_type="offert"))
            flash(errors.get("form", "Kontrollera formuläret och försök igen."), "error")

        return render_template("quote.html", form_data=form_data, errors=errors, service_titles=service_titles)

    @app.route("/kontakt", methods=["GET", "POST"])
    def contact():
        requested_reason = request.args.get("typ", "").strip()
        form_data = {**CONTACT_DEFAULTS}
        if requested_reason in CONTACT_REASONS:
            form_data["reason"] = requested_reason
        errors = {}

        if request.method == "POST":
            form_data = get_contact_form_data(request.form)
            errors = validate_contact_form(form_data)
            if not errors:
                save_submission(app.config["CONTACT_SUBMISSIONS_FILE"], form_data, [
                    "name",
                    "phone",
                    "email",
                    "subject",
                    "reason",
                    "message",
                ])
                flash("Tack! Ditt meddelande har skickats. Vi återkommer så snart vi kan.", "success")
                return redirect(url_for("thank_you", form_type="kontakt"))
            flash(errors.get("form", "Kontrollera formuläret och försök igen."), "error")

        return render_template("contact.html", form_data=form_data, errors=errors)

    @app.get("/faq")
    def faq():
        return render_template("faq.html", faq_items=FAQ_ITEMS)

    @app.get("/integritet")
    def privacy():
        return render_template("privacy.html")

    @app.get("/tack")
    def thank_you():
        form_type = request.args.get("form_type", "kontakt")
        return render_template("thank_you.html", form_type=form_type)

    @app.errorhandler(404)
    def page_not_found(_error):
        return render_template("404.html"), 404

    return app


def save_submission(path: Path, form_data: dict[str, str], fieldnames: list[str]) -> None:
    file_exists = path.exists()
    with path.open("a", encoding="utf-8", newline="") as file:
        writer = DictWriter(file, fieldnames=fieldnames)
        if not file_exists:
            writer.writeheader()
        writer.writerow({key: form_data[key] for key in fieldnames})


app = create_app()


if __name__ == "__main__":
    app.run(debug=True)
