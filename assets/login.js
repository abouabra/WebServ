document.getElementById("info_login").addEventListener("submit", function(event) {
    event.preventDefault();
    var newURL = "http://localhost:2004/index.html"
    window.location.href = newURL;
})