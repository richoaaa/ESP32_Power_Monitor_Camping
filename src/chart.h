const char GRAPH_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head><title>Chart</title>
<meta name="viewport" content="width=device-width, minimum-scale=1.0, maximum-scale=1.0, initial-scale=1" />
<script src="/jquery.js"></script>
<script src="/highcharts.js"></script>

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
</style>

<a style="font-size:20px" href="http://192.168.4.1/" class="button">Home</a><a href="/STATS_page" class="button">Stats</a> <a href="/CONFIG_page" class="button">Configuration</a>

<div id="chart-temperature" class="container"></div>
</br>
<div id="chart-Y" class="container"></div>
</br>
<div id="chart-cumulative" class="container"></div>

<script>

var chartT = new Highcharts.Chart({
  chart:{ 
    renderTo : 'chart-temperature',
    type: 'area'
  },
  title: { text: 'Live Feed' } ,
  series: [
    {
      name: 'In',
      color: '#0000FF',
      type: 'area',
      yAxis: 0,
      tooltip: {
          xDateFormat: '%k:%M:%S',
          valueSuffix: ' A'
        }
    },
    {
      name: 'Out',
      color: '#FF0000',
      type: 'area',
      yAxis: 0,
      tooltip: {
          xDateFormat: '%k:%M:%S',
          valueSuffix: ' A'
        }
    },
    {
      name: 'Volts',
      color: '#00BF00',
      type: 'line',
      yAxis: 1,
      tooltip: {
          xDateFormat: '%k:%M:%S',
          valueSuffix: ' V'
        }
    }],
  plotOptions: {
    area: { animation: false,
    dataLabels: { enabled: false },
    marker: {
                enabled: false,
                symbol: 'circle',
                radius: 2,
                states: {
                    hover: {
                        enabled: true
                    }
                }
            }
    },
    series: { 
      color: '#059e8a',
      pointIntervalUnit: 'hour'
    }
  },
  xAxis: {
    type: 'datetime',
    dateTimeLabelFormats: {
      minute: '%k:%M',
      hour: '%H:%M'
    }
  },
  yAxis: [{  // Primary yAxis
    labels: {format: '{value}'},
    title: { text: 'Amps' },
    minrange: 0.5
  }, {  // Secondary yAxis
  title: { text: 'Volts' },
  labels: {format: '{value}'},
  minrange: 0.5,
  opposite: true
  }],
  tooltip: {
        shared: true
    },
  credits: { enabled: false }
});


var chart2 = new Highcharts.Chart({
  chart:{ 
    renderTo : 'chart-Y',
    type: 'area'
  },
  title: { text: 'History' },
  series: [
    {
      name: 'In',
      data: [],
      type: 'area',
      color: '#0000FF',
      yAxis: 0,
      tooltip: {
          xDateFormat: '%k:%M',
          valueSuffix: ' A'
        }
    },
    {
      name: 'Out',
      data: [],
      type: 'area',
      color: '#FF0000',
      yAxis: 0,
      tooltip: {
          xDateFormat: '%k:%M',
          valueSuffix: ' A'
        }
    },
    {
      name: 'Volts',
      type: 'line',
     data: [],
      color: '#00BF00',
      yAxis: 1,
      tooltip: {
          xDateFormat: '%k:%M',
          valueSuffix: ' V'
        }
    }
  ],
  plotOptions: {
    area: { animation: false,
    dataLabels: { enabled: false },
    marker: {
                enabled: false,
                symbol: 'circle',
                radius: 2,
                states: {
                    hover: {
                        enabled: true
                    }
                }
            }
    },
    series: { color: '#059e8a' }
  },
  xAxis: {
    type: 'datetime',
    dateTimeLabelFormats: {
      minute: '%H:%M',
      hour: '%H:%M',
      day: 'Day %d'
    }
  },
  yAxis: [{  // Primary yAxis
    labels: {format: '{value}'},
    title: { text: 'Amps' },
    minrange: 0.5,
  }, {  // Secondary yAxis
  min: 0,
  minrange: 0.5,
  title: { text: 'Volts' },
  labels: {format: '{value}'},
  type: 'line',
  opposite: true
  }],
  credits: { enabled: false }
});


var chart3 = new Highcharts.Chart({
  chart:{ 
    renderTo : 'chart-cumulative',
    type: 'area'
  },
  title: { text: 'AMP Cumulative History' },
  series: [
    {
      name: 'In',
      data: [],
      color: '#0000FF',
    },
    {
      name: 'Out',
      data: [],
      color: '#FF0000',
    }
  ],
  tooltip: {
            type: 'datetime',
            xDateFormat: '%k:%M',
            headerFormat: '<table><tr><th colspan="2">{point.key}</th></tr>',
            pointFormat: '<tr><td style="color: {series.color}">{series.name} </td>' +
            '<td style="text-align: right"><b>{point.y} amps</b></td></tr>',
            footerFormat: '</table>',
            shared: true,
            useHTML: true,
            valueDecimals: 2
          },
  plotOptions: {
    area: { animation: false,
    dataLabels: { enabled: false },
    marker: {
                enabled: false,
                symbol: 'circle',
                radius: 2,
                states: {
                    hover: {
                        enabled: true
                    }
                }
            }
    },
    series: { color: '#059e8a' }
  },
  xAxis: {
    type: 'datetime',
    dateTimeLabelFormats: {
      minute: '%H:%M',
      hour: '%H:%M',
      day: 'Day %d %H:%M'
      }
  },
  yAxis: {
    title: { text: 'Amps' }
  },
  credits: { enabled: false }
});


setInterval(function liveData(){
  var xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var obj = JSON.parse(this.responseText);
       var x = obj.graph[0];
       var y = parseFloat(obj.graph[1]);
       var z = parseFloat(obj.graph[2]);
       var a = parseFloat(obj.graph[3]);
       if(chartT.series[0].data.length > 50) {
          chartT.series[0].addPoint([x, y], true, true, true);
          chartT.series[1].addPoint([x, z],true,true,true);
          chartT.series[2].addPoint([x, a],true,true,true);
        } else {
          chartT.series[0].addPoint([x, y], true, false, true);
          chartT.series[1].addPoint([x, z],true,false,true);
          chartT.series[2].addPoint([x, a],true,false,true);
        }
    }
  };
  xhr.open("GET", "/graphdata", true);
  xhr.send();
}, 1500 ) ;


setInterval(function AmpsIn(){
  var xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var obj = JSON.parse(this.responseText);
      console.log(obj);
      chart2.series[0].setData(obj.ampin);
    }
  };
  xhr.open("GET", "/graphdataAmpsIn", true);
  xhr.send();
}, 5000 ) ;


setInterval(function AmpsOut(){
  var xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var obj = JSON.parse(this.responseText);
      console.log(obj);
      chart2.series[1].setData(obj.ampout);
    }
  };
  xhr.open("GET", "/graphdataAmpsOut", true);
  xhr.send();
}, 5000 ) ;

setInterval(function VoltsOut(){
  var xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var obj = JSON.parse(this.responseText);
      console.log(obj);      
      chart2.series[2].setData(obj.voltsout);
    }
  };
  xhr.open("GET", "/graphdatavolts", true);
  xhr.send();
}, 5000 ) ;
  

setInterval(function AmpsInCum(){
  var xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var obj = JSON.parse(this.responseText);

      chart3.series[0].setData(obj.ampincum);
    }
  };
  xhr.open("GET", "/graphdataAmpsInCum", true);
  xhr.send();
}, 5000 ) ;


setInterval(function AmpsOutCum(){
  var xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var obj = JSON.parse(this.responseText);
      chart3.series[1].setData(obj.ampoutcum);
    }
  };
  xhr.open("GET", "/graphdataAmpsOutCum", true);
  xhr.send();
}, 5000 ) ;  




// if (!!window.EventSource) {
//   var source = new EventSource('/events');

//   source.addEventListener('open', function(e) {
//     console.log("Events Connected");
//   }, false);

//   source.addEventListener('error', function(e) {
//     if (e.target.readyState != EventSource.OPEN) {
//       console.log("Events Disconnected");
//     }
//   }, false);

//   source.addEventListener('message', function(e) {
//     console.log("message", e.data);
//   }, false);

//   source.addEventListener('new_readings', function(e) {
//     console.log("new_readings", e.data);
//     var myObj = JSON.parse(e.data);
//     console.log(myObj);
//     plotTemperature(myObj);
//   }, false);
// }


</script>
</body>
</html>

)=====";