const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>

<!-- Begin with style -->
<style>
  a{
    color: #555;
  }
  hr{
    border-color: #056016;
    background-color: #056016;
    width: 100%;
  }
  .plot{
    width: 320px;
    height: 240px;
    background-color: #777;
    border: none;
    color: #CCC;
    padding: none ;
    text-align: center;
    text-decoration: none;
    display: none;
    margin-left: auto;
    margin-right: auto;
    margin-top: 0px;
    margin-bottom: 0px;
    cursor: progress;
  }
  .plotbar {
    height: 18px;
    background-color: #666;
    display: block;
    cursor: pointer;
  }
  .button{
    border-radius: 20%;
    background-color: #056016;
    border: none;
    color: #FFF;
    padding: 3px 6px ;
    text-align: left;
    text-decoration: none;
    display: inline-block;
    margin: 3px 6px;
    cursor: pointer;
  }
  .card{
    max-width: 380px;
    min-height: 300px;
    background: #02b875;
    padding: 30px;
    padding-top: 2px;
    box-sizing: border-box;
    color: #555;
    margin:20px;
    display: block;
    margin-left: auto;
    margin-right: auto;
    box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);
    border-radius: 12px;
  }
  .lidar{
    display: none;
  }
  .signal{
    color: #02b875;
    margin:none;
    padding: none;
    border: none;
    float: right;
    font-size: 60%;
  }
</style>

<!-- Top -->
<head>
  <title>ESP32/VL53L0X display and control demo</title>
  <meta charset="utf-8" />
  <meta name="viewport" content="width=380, initial-scale=1" />
  <meta http-equiv="Cache-control" content="no-cache">
</head>

<!-- The page itself -->
<body>
  <div class="card">
    <h3 style="text-align: center;">ESP32/VL53L0X Demo
    <span id="signal" class="signal">Timeout</span></h3>
    
    <hr>
    
    <h2 style="text-align: center;">Range: <span style="font-size: 60%;">
    (mm, <span id="MODEValue">unknown</span> mode)</span></h2>
    <h1 style="text-align: center;"><span id="RANGEValue" style="color: #056016">
    Connecting..</span></h1>

    <canvas class="plot" id="plot" onclick="togglePlot()" width=320 height=240>
    This is a Canvas Element, if it is not displayed then we apologize, your browser 
    is not compatible.</canvas>
    <div class="plot plotbar" onclick="togglePlot()">Evil Plot</div>
    
    <hr>
    
    <div style="text-align: center;">
      Sensor&nbsp;&nbsp;::&nbsp;&nbsp;
      <button class="button" onclick="httpGet('/on')" 
              title="Enable sensor">On</button>
      &nbsp;&nbsp;||&nbsp;&nbsp;
      <button class="button" onclick="httpGet('/off')" 
              title="Disable sensor">Off</button>
    </div>

    <div style="text-align: center;">
      Mode&nbsp;&nbsp;::&nbsp;&nbsp;
      <button class="button" onclick="httpGet('/near')" 
              title="Near range (max 1.3m, fastest)">Near</button>
      &nbsp;&nbsp;||&nbsp;&nbsp;
      <button class="button" onclick="httpGet('/mid')" 
              title="Mid Range (max 4m, fast but less accurate at range)">Mid</button>
      &nbsp;&nbsp;||&nbsp;&nbsp;
      <button class="button" onclick="httpGet('/far')" 
              title="Far Range (max 4m, slower but more accurate over whole range)">Far</button>
    </div>

    <div style="text-align: center;">
      ROI&nbsp;&nbsp;::&nbsp;&nbsp;
      <button class="button" onclick="httpGet('/roiminus')" 
              title="Make Region of Interest smaller">Smaller</button>
      &nbsp;&nbsp;||&nbsp;&nbsp;
      <button class="button" onclick="httpGet('/roiplus')" 
              title="Make Region of Interest bigger">Bigger</button>
    </div>

    <div style="text-align: center;" class="lidar">
      Turret&nbsp;&nbsp;::&nbsp;&nbsp;
      <button class="button" onclick="httpGet('/left')" 
              title="Swing the sensor left">left</button>
      &nbsp;&nbsp;||&nbsp;&nbsp;
      <button class="button" onclick="httpGet('/right')" 
              title="Swing the sensor right">Right</button>
    </div>

    <span onclick="toggleStatusPanel()">
      <h3 style="text-align: center; cursor: pointer; text-decoration: underline;">
      Show Status</h3>
      <pre><span id="INFOValue" style="color: #056016; display: none;">
      Connecting..</span></pre>
    </span>
    <p style="text-align: center;">::<a href="https://github.com/easytarget/esp32-rangefinder-ajax-demo" 
    title="Project Home">GitHub</a>::</p>
  </div>

  <!-- The scripting -->
  
  <script>
  
    // Create the recurring interval task to refresh the distance value
    setInterval(function() {
      getValues();
    }, 200); // interval in ms.

    // Create the recurring interval task to refresh the info data 
    setInterval(function() {
      getInfo();
    }, 500); // interval in ms.

    // make a simple http request, return result, used to trigger actions from buttons
    function httpGet(theUrl)
    {
        var xmlHttp = new XMLHttpRequest();
        xmlHttp.open( "GET", theUrl, false ); // false for synchronous request
        xmlHttp.send( null );
        return xmlHttp.responseText;
    }

    // Loop to get, process and display value readings
    var plotline = 0;
    var plotscale = 20;
    function getValues() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          var response = JSON.parse(this.responseText);
          document.getElementById("signal").style.color = "#02b875";
          if (response.RangeStatus < 0) {
            document.getElementById("RANGEValue").innerHTML = "Disabled";
            document.getElementById("RANGEValue").style.color = "#555";
          } else {
            switch (response.RangeStatus) {
                case 0:
                  document.getElementById("RANGEValue").innerHTML = response.Distance;
                  document.getElementById("RANGEValue").style.color = "#056016";
                break;
                case 1:
                  document.getElementById("RANGEValue").innerHTML = "Sigma Fail";
                  document.getElementById("RANGEValue").style.color = "#8e0b0b";
                break;
                case 2:
                  document.getElementById("RANGEValue").innerHTML = "Signal fail";
                  document.getElementById("RANGEValue").style.color = "#8e0b0b";
                break;
                case 4:
                  document.getElementById("RANGEValue").innerHTML = "Out of Bounds";
                  document.getElementById("RANGEValue").style.color = "#8e0b0b";
                break;
                case 5:
                  document.getElementById("RANGEValue").innerHTML = "Hardware Fail";
                  document.getElementById("RANGEValue").style.color = "#8e0b0b";
                break;
                case 7:
                  document.getElementById("RANGEValue").innerHTML = "Wrapped target";
                  document.getElementById("RANGEValue").style.color = "#8e0b0b";
                break;
                case 8:
                  document.getElementById("RANGEValue").innerHTML = "Internal Error";
                  document.getElementById("RANGEValue").style.color = "#8e0b0b";
                break;
                case 14:
                  document.getElementById("RANGEValue").innerHTML = "Invalid Result";
                  document.getElementById("RANGEValue").style.color = "#8e0b0b";
                break;
                case 255:
                  document.getElementById("RANGEValue").innerHTML = "No Response";
                  document.getElementById("RANGEValue").style.color = "#8e0b0b";
                break;
                default:
                  document.getElementById("RANGEValue").innerHTML = "Unknown Error";
                  document.getElementById("RANGEValue").style.color = "#8e0b0b";
            }
          }
          if (response.Angle) {
            document.getElementById("RANGEValue").innerHTML += response.Angle;
          }        
        
          if (response.RangeStatus == 0) {
            ctx = document.getElementById("plot").getContext("2d");
            var maxX = Math.floor(ctx.canvas.width);
            var maxY = Math.floor(ctx.canvas.height);
            plotvalue = Math.floor(response.Distance / plotscale);
            plotpoint = maxY - plotvalue;
            ctx = document.getElementById("plot").getContext("2d");
            ctx.fillStyle = "#DDDDDD";
            ctx.fillRect(plotline, plotpoint, 1, plotvalue);
            plotline = plotline + 1;
            if (plotline > maxX) {
              plotline = 0;
            }
            ctx.clearRect(plotline, 0, 1, maxX);
          }
        }
      }
      xhttp.open("GET", "/range", true);
      xhttp.send();
      xhttp.timeout = 300; // time in milliseconds
      xhttp.ontimeout = function () {
        document.getElementById("signal").style.color = "#8e0b0b";
      };
    }
    
    // Read and process the extended info, handle mode changes
    var mode = "NONE";
    function getInfo() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          var response = JSON.parse(this.responseText);
          document.getElementById("INFOValue").innerHTML = this.responseText;
          if (response.Mode != mode) {
            mode = response.Mode;
            setMode(response.Mode);
          }
        }
      }
      xhttp.open("GET", "/info", true);
      xhttp.send();
      xhttp.timeout = 600; // time in milliseconds
    };

    var plot = false;
    function togglePlot() {
      if (plot) { //collapse
        document.getElementById("plot").style.display = "none";
        plot = false;
      } else { // expand
        document.getElementById("plot").style.display = "block";
        plot = true;
      }
    }
    
    function toggleStatusPanel() {
      var x = document.getElementById("INFOValue");
      if (x.style.display === "none") {
        x.style.display = "block";
      } else {
        x.style.display = "none";
      }
    }

    // Updates page to show the default-hidden lidar controls
    function enableLidar() {
      var x = document.getElementsByClassName("lidar");
      var i;
      for (i = 0; i < x.length; i++) {
        x[i].style.display = block;
      }
    }

    // Called when a mode change is detected; sets the graph etc.
    function setMode(mode) {
      ctx = document.getElementById("plot").getContext("2d");
      var maxX = Math.floor(ctx.canvas.width);
      var maxY = Math.floor(ctx.canvas.height);
      ctx.clearRect(0, 0, maxX, maxY);
      plotline = 0;
      switch (mode) {
        case "Near":
          plotscale = 8;
          document.getElementById("MODEValue").innerHTML = "Near (1.3m)";
          break;
        case "Mid":
          plotscale = 16;
          document.getElementById("MODEValue").innerHTML = "Mid (2.6m)";
          break;
        case "Far":
          plotscale = 20;
          document.getElementById("MODEValue").innerHTML = "Far (4m)";
          break;
        default:
          plotscale = 100;
          document.getElementById("MODEValue").innerHTML = "Unknown";
      }
    }

  </script>
</body>
</html>
)=====";
