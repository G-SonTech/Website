document.documentElement.classList.add("js");

const menuToggle = document.querySelector(".menu-toggle");
const siteNav = document.querySelector(".site-nav");

if (menuToggle && siteNav) {
  menuToggle.addEventListener("click", () => {
    const expanded = menuToggle.getAttribute("aria-expanded") === "true";
    menuToggle.setAttribute("aria-expanded", String(!expanded));
    siteNav.classList.toggle("is-open");
  });
}

const observer = new IntersectionObserver(
  (entries) => {
    entries.forEach((entry) => {
      if (entry.isIntersecting) {
        entry.target.classList.add("is-visible");
      }
    });
  },
  { threshold: 0.12 }
);

document
  .querySelectorAll(".card, .promise-card, .mini-card, .info-panel, .contact-form, .faq-item")
  .forEach((element) => observer.observe(element));

document.querySelectorAll(".faq-question").forEach((button) => {
  button.addEventListener("click", () => {
    const item = button.closest(".faq-item");
    const isOpen = item.classList.toggle("is-open");
    button.setAttribute("aria-expanded", String(isOpen));
  });
});

function serializeForm(form) {
  const formData = new FormData(form);
  return new URLSearchParams(formData).toString();
}

document
  .querySelectorAll("form[data-netlify='true']")
  .forEach((form) => {
    form.addEventListener("submit", async (event) => {
      event.preventDefault();

      const action = form.getAttribute("action") || "/tack/";
      const body = serializeForm(form);

      try {
        const response = await fetch("/", {
          method: "POST",
          headers: { "Content-Type": "application/x-www-form-urlencoded" },
          body,
        });

        if (response.ok) {
          window.location.assign(action);
          return;
        }
      } catch (_error) {
        // Fall through to browser default submit.
      }

      form.submit();
    });
  });
