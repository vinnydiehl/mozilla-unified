function handleRequest(request, response)
{
  response.setStatusLine(null, 401, "Unauthorized");
  response.setHeader("WWW-Authenticate", "basic realm=\"restricted\"", false);
}
