var ToC =
  "<nav role='navigation' class='table-of-contents'>" +
    "<h3>Contents:</h3>" +
    "<ul>";

var newLine, el, title, link;

function addToList(element) {
  title = element.text();
  link = "#" + element.attr("id");
  newLine =
    "<li>" +
      "<a href='" + link + "'>" +
        title +
      "</a>" +
    "</li>";
    ToC += newLine;
}

$("h1, h2").each(function() {
  addToList($(this));
});

ToC +=
   "</ul>" +
  "</nav>";

$(".my_table_of_contents").prepend(ToC);