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

Highcharts.setOptions({
  global: {
    useUTC: false
    // timezoneOffset: 8 * 60 // UTC+8 in minutes
  }
});

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
          xDateFormat: '%I:%M:%S %p',
          valueSuffix: ' A'
        }
    },
    {
      name: 'Out',
      color: '#FF0000',
      type: 'area',
      yAxis: 0,
      tooltip: {
          xDateFormat: '%I:%M:%S %p',
          valueSuffix: ' A'
        }
    },
    {
      name: 'Volts',
      color: '#00BF00',
      type: 'line',
      yAxis: 1,
      tooltip: {
          xDateFormat: '%I:%M:%S %p',
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
   tooltip: {
        xDateFormat: '%a %I:%M:%S %p', // Specify the date format for the tooltip
        valueSuffix: ' A'
    },
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
          valueDecimals: 1, // Adjust the number of decimals as needed
          xDateFormat: '%a %I:%M:%S %p',
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
          valueDecimals: 1, // Adjust the number of decimals as needed
          xDateFormat: '%a %I:%M:%S %p',
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
          valueDecimals: 1, // Adjust the number of decimals as needed
          xDateFormat: '%a %I:%M:%S %p',
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
      day: '%a'
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

  var chart3 = new Highcharts.Chart({
    chart: {
      // timezone: 'Australia/Perth',
      renderTo : 'chart-cumulative',
      animation: false,
      zoomType: 'x',
      type: 'column'
    },
    rangeSelector: false,
    title: {
      text: 'Daily Performance'
    },
    plotOptions: {
      column: {
        stacking: 'normal',
        turboThreshold: 0, // Disable turboThreshold for column series
        tooltip: {
          valueDecimals: 0, // Adjust the number of decimals as needed
          valueSuffix: ' Ah', // Add your desired value suffix
        },
      },
    },
    xAxis: {
      gapSize: 1,
      type: 'datetime',
      dateTimeLabelFormats: {
        day: '%a'
      },
      // labels: {
      //   formatter: function() {
      //     return Highcharts.dateFormat('%d %b', this.value); // Use your desired date format
      // }
      // }
      // minRange: 24 * 3600 * 1000, // Minimum range of one day
    },
    yAxis: {
      title: {
        text: 'Amp Hours (Ah)',
      },
    },
    legend: {
      reversed: true
    },
    series: [
      {
        name: 'Amp Hours In',
        color: 'blue', // Customize the color for Amp Hours In
        // pointInterval: 24 * 3600 * 1000 // One day in milliseconds
      },
      {
        name: 'Amp Hours Out',
        color: 'red', // Customize the color for Amp Hours Out
        // pointInterval: 24 * 3600 * 1000 // One day in milliseconds
      },
    ],
    credits: {
      enabled: false
    }
  });


setInterval(function liveData(){
  var xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var obj = JSON.parse(this.responseText);
      var x = obj.graph[0] * 1000;
      // console.log("time: ", x);
      var y = parseFloat(obj.graph[1]);
      var z = parseFloat(obj.graph[2]);
      var a = parseFloat(obj.graph[3]);
      if(chartT.series[0].data.length > 50) {
        chartT.series[0].addPoint({ x: x, y: y }, true, true, true);
        chartT.series[1].addPoint({ x: x, y: z }, true, true, true);
        chartT.series[2].addPoint({ x: x, y: a }, true, true, true);
      } else {
        chartT.series[0].addPoint({ x: x, y: y }, true, false, true);
        chartT.series[1].addPoint({ x: x, y: z }, true, false, true);
        chartT.series[2].addPoint({ x: x, y: a }, true, false, true);
      }
    }
  };
  xhr.open("GET", "/graphdata", true);
  xhr.send();
}, 1500 ) ;

setInterval(function DataGraph2(){
  var xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var obj = JSON.parse(this.responseText);
      // console.log(obj);

      // Convert epoch time values in seconds to milliseconds
      obj[0].ampin.forEach(function(dataPoint) {
        dataPoint[0] *= 1000; // Convert seconds to milliseconds
      });

      obj[1].ampout.forEach(function(dataPoint) {
        dataPoint[0] *= 1000; // Convert seconds to milliseconds
      });

      obj[2].volts.forEach(function(dataPoint) {
        dataPoint[0] *= 1000; // Convert seconds to milliseconds
      });

      console.log(obj[0].ampin);
      chart2.series[0].setData(obj[0].ampin);
      chart2.series[1].setData(obj[1].ampout);
      chart2.series[2].setData(obj[2].volts);
    }
  };
  xhr.open("GET", "/graphdata2", true);
  xhr.send();
}, 5000 ) ;



setInterval(function AmpHistory(){
  var xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var obj = JSON.parse(this.responseText);

      // Convert epoch time values in seconds to milliseconds
      obj[0].amphrin.forEach(function(dataPoint) {
        dataPoint[0] *= 1000; // Convert seconds to milliseconds
      });

      obj[1].amphrout.forEach(function(dataPoint) {
        dataPoint[0] *= 1000; // Convert seconds to milliseconds
      });

      chart3.series[0].setData(obj[0].amphrin);
      chart3.series[1].setData(obj[1].amphrout);
    }
  };
  xhr.open("GET", "/graphdataAh", true);
  xhr.send();
}, 6000 ) ;




</script>
</body>
</html>

)=====";