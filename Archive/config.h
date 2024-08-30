const char CONFIG_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head><title>Configuration</title>
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

<p><big> Configuration</big></p>
<p><a style="font-size:20px" href="http://192.168.4.1/" class="button">Home</a><a href="/STATS_page" class="button">Stats</a><a href="/update" class="button">Update</a></p>
<br/>

<form method="post" enctype=\"application/x-www-form-urlencoded\" id="config_frm">   

    <div class='form-floating'>
    <table  align="center">
       <tr>
        <td><label for="battery">Battery Size (Ah)</label></td>
        <td><input for='number' class='form-control' maxlength="3" max="255" size="3" name='battery' id ="battery" pattern="\d*" inputmode="numeric" type="text"></td>
      </tr>
      <br/>
       <tr>
        <td align="right"><label for="battery">Charge Efficiency (%)</label></td>
        <td><input for='number' class='form-control' maxlength="3" max="255" size="3" min="1" max="100" name='ChargeEfficiency' id ="ChargeEfficiency" pattern="\d*" inputmode="numeric" type="text"></td>
      </tr>
     </table>    
     </div>

    <br/>

	<div class='form-floating'>
    <table  align="center">
      <tr align="right">
        <td>Reset SOC</td><br/>
        <td>
          <label class="toggle" >
            <span class="onoff">OFF</span>
            <input type="checkbox" class='form-control' id="Reset" name="Reset">
            <span class="slider round" checked ></span>
          </label>
        </td>
      </tr>
      </table>
    </div>

    <div class='form-floating'>  
      <tr><td></td></tr>
      <br/>
      <tr>
        <td><label for="appt">Set Clock:</label></td>
        <td><input type="time" id="clockID" name="clockID"></td>
      </tr>
    </div>

    <div class='form-floating'>  
      <tr><td><br/></td></tr>
      <tr>
        <td align="center" colspan="2"><button type="submit" class = "button" id = "btn1" >Save</button></td>  
      </tr>
    </div>
  </table>
</form>

  <table align="center">
    <div class='form-2'>  
      <tr><td><br/></td></tr>
      <tr>
        <td><button class = "buttonSmall" id = "btnReboot" onclick="ButtonReboot()">Reboot</button></td>
      </tr>
    </div>
  </table>
<p><a Free Memory <span id="Memory"</a> </p>


<script type = "text/javascript">

const toggle = document.querySelector('.toggle input')

toggle.addEventListener('click', () => {
    const onOff = toggle.parentNode.querySelector('.onoff')
    onOff.textContent = toggle.checked ? 'ON' : 'OFF'
})

function loadDoc() {
    var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        var obj = JSON.parse(this.responseText);
        document.getElementById("battery").value = obj.data[0].dataValue;
        document.getElementById("ChargeEfficiency").value = obj.data[1].dataValue;
        document.getElementById("Memory").innerHTML = obj.data[2].dataValue;   
      }
    };
    xhttp.open("GET", "/config", true); xhttp.send();
  }
window.onload=loadDoc();

  
function ButtonSave() {
      var xhttp = new XMLHttpRequest(); 
      xhttp.open("PUT", "SaveConfigButton", false);
      xhttp.send();
      confirm("Updated!");
      }

function ButtonReboot() {
  if (confirm("Reboot microcontroller?")) { 
    var xhttp = new XMLHttpRequest(); 
    xhttp.open("PUT", "RebootButton", false);
    xhttp.send();
  }
}

function ZeroLevel() {
      if (confirm("Calibrate Level?")) { 
        var xhttp = new XMLHttpRequest(); 
        xhttp.open("PUT", "ZeroLevelButton", false);
        xhttp.send();
        }
      }

// function SetClock() {
//       var xhttp = new XMLHttpRequest(); 
//       xhttp.open("PUT", "SetClockButton", false);
//       xhttp.send();
//       confirm("Updated!");
//       }

// function to handle the response from the ESP
    function response(){
      var message;
      var color = "#e8e8e8";
      // get the xml stream
      xmlResponse=xmlHttp.responseXML;
      if (message > 2048){
      color = "#aa0000";
      }
      else {
        color = "#0000aa";
      } 
    }

    function process(){
     if(xmlHttp.readyState==0 || xmlHttp.readyState==4) {
        xmlHttp.open("PUT","xml",true);
        xmlHttp.onreadystatechange=response;
        xmlHttp.send(null);
      }       
        // you may have to play with this value, big pages need more porcessing time, and hence
        // a longer timeout
        setTimeout("process()",100);
    }

  
</script>
</body>
</html>

)=====";