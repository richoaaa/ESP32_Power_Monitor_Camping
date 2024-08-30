const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head><title>Power Monitor</title>
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
<p><big>Camper Mate</big></p>
<p><a href="/STATS_page" class="button">Stats</a> <a href="/GRAPH_page" class="button">Graphs</a> <a href="/CONFIG_page" class="button">Configuration</a></p>

<div id="battery"></div>

<a class="SOC_style"> SOC: <span id="SOC"></span>%</a>
</br>

<table align="center" cellpadding="5">
<tbody align="left">
<tr>
  <td><img src="\Battery2.png" width="30" height="30"></td>
  <td>Battery Volts </td>
  <td><span id="volts" style="color:#0000FF"></span></td>
  <td><img src="\solar.png" width="30" height="30"></td>
  <td> Solar </td>
  <td><span id="solarAmps" style="color:#0000FF"></span></td>
</tr>
<tr>
  <td><img src="\Charging.png" width="30" height="30"></td>
  <td>Charger  </td>
  <td><span id="chargerAmps" style="color:#0000FF"></span></td>
  <td><img src="\load.png" width="30" height="30"></td>
  <td> Load </td>
  <td><span id="load" style="color:#0000FF"></span></td>
</tr>
<tr>
  <td><img src="\Charging.png"" width="30" height="30"></td>
  <td>Ah In  </td>
  <td><span id="amphIn" style="color:#0000FF"></span></td>
  <td><img src="\Discharging.png" width="30" height="30"></td>
  <td> Ah Out </td>
  <td><span id="amphOut" style="color:#0000FF"></span></td>
</tr>
</tbody>
<hr>

<table align="center" cellpadding="10">
<tr>
  <td align="right"><img src="\temperature.png" width="30" height="30"></td>
  <td align="left">Temperature <span id="Temp1"></span>&#176;C </td>
</tr>
<tr>
  <td align="right"><img src="\clock.png"></td>
  <td align="left"> Server Time <span id="ServerTime"></span> (<span id="ServerUpTime"></span>)</td>
</tr>
</table>

</br>
Currently <span id="status"></span>
</br>
Overall State: <span id="message"></span>

<p><a href="/TEST_page" class="button">Test</a></p>


<script>
  
    function loadDoc() {
    var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        var obj = JSON.parse(this.responseText);
        document.getElementById("volts").innerHTML = obj.data[0].dataValue;
        document.getElementById("solarAmps").innerHTML = obj.data[1].dataValue;
        document.getElementById("chargerAmps").innerHTML = obj.data[2].dataValue;
        document.getElementById("load").innerHTML = obj.data[3].dataValue;
        document.getElementById("SOC").innerHTML = obj.data[4].dataValue;
        document.getElementById("Temp1").innerHTML = obj.data[5].dataValue;
        document.getElementById("amphIn").innerHTML = obj.data[6].dataValue;
        document.getElementById("amphOut").innerHTML = obj.data[7].dataValue;
        document.getElementById("ServerTime").innerHTML = obj.data[8].dataValue;
        document.getElementById("message").innerHTML = obj.data[9].dataValue;
        document.getElementById("status").innerHTML = obj.data[10].dataValue;
        document.getElementById("ServerUpTime").innerHTML = obj.data[11].dataValue;
      }
    };
    xhttp.open("GET", "/data", true); xhttp.send();
  }


  var timedEvent = setInterval(function(){ loadDoc(); }, 1500);

  function outputUpdate(vol) {
    timedEvent = setInterval(function(){ loadDoc(); }, 1500);
  }
  

</script>
</body>
</html>

)=====";
