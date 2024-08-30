const char STATS_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head><title>Power Configuration</title>
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

  .tableformat{
  border-collapse: collapse;
  border: 3px solid black;
  }
</style>

<body>

  <p><big>Records</big></p>
  <p><a style="font-size:20px" href="http://192.168.4.1/" class="button">Home</a>  <a href="/GRAPH_page" class="button">Graphs</a> <a href="/CONFIG_page" class="button">Configuration</a></p>


<table align="center" class=tableformat border="1">
<tbody align="left" >
<tr>
  <th>Item</th>
  <th>Record</th>
  <th>Time</th>
</tr>
<tr>
  <td style="width:50%">Max Amps In</td>
  <td style="width:20%"><span id="MaxAmpIn"></td>
  <td style="width:20%"><span id="MaxAmpInTime"></td>
</tr>
<tr>
  <td>Max Amps Out</td>
  <td><span id="MaxAmphOut"></td>
  <td><span id="MaxAmphOutTime"></td>
  </tr>
<tr>
  <td>Min Volts</td>
  <td><span id="MinVolts"></td>
  <td><span id="MinVoltsTime"></td>
  </tr>
<tr>
  <td>Max Volts</td>
  <td><span id="MaxVolts"></td>
  <td><span id="MaxVoltsTime"></td>
</tr>
<tr>
  <td>Min SOC</td>
  <td><span id="MinSOC"></td>
  <td><span id="MinSOCTime"></td>
</tr>
<tr>
  <td>Max SOC</td>
  <td><span id="MaxSOC"></td>
  <td><span id="MaxSOCTime"></td>
</tr>
<tr>
  <td>Internal temperature Min</td>
  <td><span id="FridgeMin">&#176;</td>
  <td><span id="FridgeMinTime"></td>
</tr>
<tr>
  <td>Internal temperature Max</td>
  <td><span id="FridgeMax">&#176;</td>
  <td><span id="FridgeMaxTime"></td>
</tr>
<tr>
  <td>External temperature Min</td>
  <td><span id="RadMin">&#176;</td>
  <td><span id="RadMinTime"></td>
</tr>
<tr>
  <td>External temperature Max</td>
  <td><span id="RadMax">&#176;</td>
  <td><span id="RadMaxTime"></td>
</tr>


</tbody>
</table>

</br>
<table align="center">
<tr>
  <td align="right"><img src="\clock.png"></td>
  <td align="left">Time: <span id="ServerUpTime"></span></td>
</tr>
</table>

<form method="post" enctype=\"application/x-www-form-urlencoded\" id="stats_frm">
    <div class='form-floating'><br/>
      <button type="submit" class = "btn" id = "btn1" onclick="ButtonReset()">Reset Stats</button>
    </div>
</form>

<p><a href="/GRAPH_page" class="button">Graphs</a> </p>

<script>

function loadDoc() {
    var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        var obj = JSON.parse(this.responseText);
        document.getElementById("MaxAmpIn").innerHTML = obj.data[0].dataValue;
        document.getElementById("MaxAmpInTime").innerHTML = obj.data[1].dataValue;
        document.getElementById("MaxAmphOut").innerHTML = obj.data[2].dataValue;
        document.getElementById("MaxAmphOutTime").innerHTML = obj.data[3].dataValue;
        document.getElementById("MinVolts").innerHTML = obj.data[4].dataValue;
        document.getElementById("MinVoltsTime").innerHTML = obj.data[5].dataValue;
        document.getElementById("MaxVolts").innerHTML = obj.data[6].dataValue;
        document.getElementById("MaxVoltsTime").innerHTML = obj.data[7].dataValue;
        document.getElementById("MinSOC").innerHTML = obj.data[8].dataValue;
        document.getElementById("MinSOCTime").innerHTML = obj.data[9].dataValue;
        document.getElementById("MaxSOC").innerHTML = obj.data[10].dataValue;
        document.getElementById("MaxSOCTime").innerHTML = obj.data[11].dataValue;
        document.getElementById("FridgeMin").innerHTML = obj.data[12].dataValue;
        document.getElementById("FridgeMinTime").innerHTML = obj.data[13].dataValue;
        document.getElementById("FridgeMax").innerHTML = obj.data[14].dataValue;
        document.getElementById("FridgeMaxTime").innerHTML = obj.data[15].dataValue;
        document.getElementById("RadMin").innerHTML = obj.data[16].dataValue;
        document.getElementById("RadMinTime").innerHTML = obj.data[17].dataValue;
        document.getElementById("RadMax").innerHTML = obj.data[18].dataValue;
        document.getElementById("RadMaxTime").innerHTML = obj.data[19].dataValue;
        document.getElementById("ServerUpTime").innerHTML = obj.data[20].dataValue;   
      }
    };
    xhttp.open("GET", "/stats", true); xhttp.send();
  }

var timedEvent = setInterval(function(){ loadDoc();}, 1000);

  function outputUpdate(vol) {
    timedEvent = setInterval(function(){ loadDoc();}, 1000);
  }
function ButtonReset() {
  var xhttp = new XMLHttpRequest(); 
      /*
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("button1").innerHTML = this.responseText;
        }
      }
      */
      confirm("Updated!");
      xhttp.open("PUT", "BUTTON_RST", false);
      xhttp.send();
    }

    var timedEvent = setInterval(function(){ loadDoc(); }, 500);

  function outputUpdate(vol) {
    timedEvent = setInterval(function(){ loadDoc(); }, 500);
  }

</script>
</body>
</html>

)=====";
