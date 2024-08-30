const char UPDATE_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head><title>Trailer Mate</title>
<meta name="viewport" content="width=device-width, minimum-scale=1.0, maximum-scale=1.0, initial-scale=1" />
<style>
  body {font-family:Arial; text-align: center; max-width: 600px; margin: 10px auto; background-color:#adbddf; }

 .button {
  background-color: #ddd;
  border: none;
  color: black;
  padding: 10px 20px;
  text-align: right;
  text-decoration: none;
  display: inline-block;
  margin: 4px 2px;
  cursor: pointer;
  border-radius: 16px;
  }

  .button:hover {
  background-color: #f1f1f1;
  }

  hr {
  width: 75%;
  height: 0.5px;
  background-color: black;
  margin-right: auto;
  margin-left: auto;
  margin-top: 0px;
  margin-bottom: 10px;
  border-width: 1px;
  border-color: black;
}

.SOC_style {
	color:blue;
  font-weight: bold;
  }

</style>

</head>
<body>
<p><big>Update</big></p>
<p><a style="font-size:20px" href="http://192.168.4.1/" class="button">Home</a><a href="/STATS_page" class="button">Stats</a> <a href="/CONFIG_page" class="button">Configuration</a></p>

<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>

</body>
</html>

)=====";
