const char TEST_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head><title>Test Page</title>
<meta name="viewport" content="width=device-width, minimum-scale=1.0, maximum-scale=2.0, initial-scale=1" />
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
  
  .buttonSmall {
  background-color: #ddd;
  border: none;
  color: black;
  padding: 5px 10px;
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
  
   .buttonSmall:hover {
  background-color: #f1f1f1;
  }
  
    .toggleSwitch {
    display: inline-block;
    height: 18px;
    position: relative;
    overflow: visible;
    padding: 0;
    margin-left: 50px;
    cursor: pointer;
    width: 40px;
      user-select: none;
  }
 
.slider {
  position: absolute;
  cursor: pointer;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background-color: #ccc;
  -webkit-transition: .4s;
  transition: .4s;
}

.slider:before {
  position: absolute;
  content: "";
  height: 14px;
  width: 14px;
  left: 4px;
  bottom: 4px;
  background-color: white;
  -webkit-transition: .4s;
  transition: .4s;
}

input:checked + .slider {
  background-color: #2196F3;
}

input:focus + .slider {
  box-shadow: 0 0 1px #2196F3;
}

input:checked + .slider:before {
  -webkit-transform: translateX(26px);
  -ms-transform: translateX(26px);
  transform: translateX(46px);
}

/* Rounded sliders */
.slider.round{
  border-radius: 34px;
}

.slider.round:before {
  border-radius: 50%;
}
.toggle {
  position: relative;
  display: inline-block;
  width: 70px;
  height: 20px;
}

.toggle input {

  opacity: 0;
  width: 0;
  height: 0;
}

.toggle .onoff {
  color: #fff;
  font-size: 0em;
  text-align: center;
  display: block;
  font-family: Arial, Helvetica, sans-serif;
}
</style>

<body>

<p><big> Test Page</big></p>
<p><a style="font-size:20px" href="http://192.168.4.1/" class="button">Home</a></p>
<br/>

<table align="center" class=tableformat border="1">
<tbody align="left" >
<tr>
  <td align="left">Battery Volts (adc) </td>      
  <td align="left"><span id="volts"></span></td>
</tr>
<tr>
  <td align="left">Charger id="charger"></span> </td>
</tr>
<tr>
  <td align="left">Solar id="solar"></span> </td>
</tr>
<tr>
  <td align="left">Amps Out id="ampsout"></span> </td>
</tr>
<tr>
  <td align="left">Free Memory</td>
  <td align="left"> <span id="freemon"></span> </td>
</tr>
</tbody>
  </table>


<script type = "text/javascript">

function loadDoc() {
    var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        var obj = JSON.parse(this.responseText);
        document.getElementById("volts").innerHTML = obj.data[0].dataValue;
        document.getElementById("charger").innerHTML = obj.data[1].dataValue;
        document.getElementById("solar").innerHTML = obj.data[2].dataValue;
        document.getElementById("ampsout").innerHTML = obj.data[3].dataValue;
        document.getElementById("freemon").innerHTML = obj.data[4].dataValue;
      }
    };
    xhttp.open("GET", "/test", true); xhttp.send();
  }

var timedEvent = setInterval(function(){ loadDoc();}, 1000);

  function outputUpdate(vol) {
    timedEvent = setInterval(function(){ loadDoc();}, 1000);
  }


  
</script>
</body>
</html>

)=====";