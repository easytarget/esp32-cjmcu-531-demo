const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>

<!-- Begin with style -->
<style>
  a{
    color: #555;
    text-decoration: none;
  }
  hr{
    border-color: #056016;
    background-color: #056016;
    width: 100%;
  }
  .canvases{
    width: 320px;
    height: 240px;
    background-color: #777;
    border: none;
    color: #CCC;
    padding: none ;
    text-align: center;
    display: none;
    margin-left: auto;
    margin-right: auto;
    margin-top: 0px;
    margin-bottom: 0px;
    cursor: progress;
  }
  .button{
    border-radius: 20%;
    background-color: #056016;
    border: none;
    color: #FFF;
    padding: 3px 6px ;
    text-align: left;
    display: inline-block;
    margin: 3px 6px;
    cursor: pointer;
    outline-style:none;
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
    outline-style:none;
    text-decoration: none;
  }
  .expander{
    text-align: center;
    cursor: pointer;
    color: #555;
  }
  .lidar{
    display: none;
  }
  .timeout{
    color: #FF7400;
    margin:none;
    padding: none;
    border: none;
    float: right;
    font-size: 100%;
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
    <hr> 
    <div id="signal" class="signal">Comms Timeout</div>
    <div><span id="RANGEValue" style="float: right; font-size: 200%;font-weight: bold;">?</span>
      <span style="float: left; font-size: 160%;font-weight: bold;">Range 
      <span style="font-size: 80%;"> (mm) </span>:</span></div><hr>
    <div><span id="MODEValue"style="float: right; font-weight: bold;">unknown</span>&nbsp;
      <span style="float: left;font-weight: bold;">Mode:</span></div>
    <div class="lidar"><hr><span id="ANGLEValue" style="float: right; font-weight: bold;">
      Unknown</span>
      <span style="float: left; font-weight: bold;">Lidar Angle
      <span style="font-size: 90%;"> (&deg;) </span>:</span></div><hr>
      
    <span class="lidar"><canvas class="canvases" id="scan" onclick="hideScan()" width=320 height=240>
    This is a Canvas Element, if it is not displayed then we apologize, your browser 
    is not compatible.</canvas>
    <h4 class="expander" id="scanControl" onclick="showScan()">show scan</h4></span>
    <canvas class="canvases" id="plot" onclick="hidePlot()" width=320 height=240>
    This is a Canvas Element, if it is not displayed then we apologize, your browser 
    is not compatible.</canvas>
    <h4 class="expander" id="plotControl" onclick="showPlot()">show history</h4>

    <hr>
    <div style="text-align: left;">
      Sensor&nbsp;&nbsp;::&nbsp;&nbsp;
      <button class="button" onclick="httpGet('/on')"
              title="Enable sensor">On</button>
      &nbsp;&nbsp;||&nbsp;&nbsp;
      <button class="button" onclick="httpGet('/off')"
              title="Disable sensor">Off</button>
    </div>
    <div style="text-align: left;">
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
    <div style="text-align: left;" class="lidar">
      Lidar&nbsp;&nbsp;::&nbsp;&nbsp;
      <button class="button" onclick="httpGet('/s-left')" 
              title="Swing the sensor left">Left</button>
      &nbsp;&nbsp;||&nbsp;&nbsp;
      <button class="button" onclick="httpGet('/s-home')"
              title="Go to the Home (zero) position and turn stepper off">Home</button>
      &nbsp;&nbsp;||&nbsp;&nbsp;
      <button class="button" onclick="httpGet('/s-right')" 
              title="Swing the sensor right">Right</button>
    </div>
    <div>
      <h3 onclick="showControlPanel()" id="showControl" 
        class="expander">
      more control</h3>
      <span id="controlPanel" style="color: #056016; display: none;">
        <h3 onclick="hideControlPanel()" class="expander">
          less control
        </h3>
        <div style="text-align: left;">
          RegionOfInterest&nbsp;&nbsp;::&nbsp;&nbsp;
          <button class="button" onclick="httpGet('/roiminus')" 
             title="Make Region of Interest smaller">Smaller</button>
          &nbsp;&nbsp;||&nbsp;&nbsp;
          <button class="button" onclick="httpGet('/roiplus')" 
             title="Make Region of Interest bigger">Bigger</button>
        </div>
        <div style="text-align: left;">
          TimingBudget&nbsp;&nbsp;::&nbsp;&nbsp;
          <button class="button" onclick="httpGet('/budgetminus')" 
             title="Make Timing Budget smaller">Smaller</button>
          &nbsp;&nbsp;||&nbsp;&nbsp;
          <button class="button" onclick="httpGet('/budgetplus')" 
             title="Make Timing Budget bigger">Bigger</button>
        </div>
        <div style="text-align: left;">
          Interval&nbsp;&nbsp;::&nbsp;&nbsp;
          <button class="button" onclick="httpGet('/intervalminus')" 
             title="Make InterMeasurement period smaller">Smaller</button>
          &nbsp;&nbsp;||&nbsp;&nbsp;
          <button class="button" onclick="httpGet('/intervalplus')" 
             title="Make InterMeasurement period bigger">Bigger</button>
        </div>
        <div style="text-align: left;" class="lidar">
          Step Delta&nbsp;&nbsp;::&nbsp;&nbsp;
          <button class="button" onclick="httpGet('/s-deltaminus')" 
             title="Make step delta angle smaller">Smaller</button>
          &nbsp;&nbsp;||&nbsp;&nbsp;
          <button class="button" onclick="httpGet('/s-deltaplus')" 
             title="Make step delta angle bigger">Bigger</button>
        </div>
        <div style="text-align: left;" class="lidar">
          Step Control&nbsp;&nbsp;::&nbsp;&nbsp;
          <button class="button" onclick="httpGet('/s-off')" 
             title="Power Down the stepper">Off</button>
          &nbsp;&nbsp;||&nbsp;&nbsp;
          <button class="button" onclick="httpGet('/s-zero')" 
             title="Declare new Zero position">Zero</button>
        </div>
      </span>
    </div>
    <span id="statusPanel" style="color: #056016; display: none;">
    <pre style="font-weight: bold;">Status::</pre>
    <pre id="statusText">Connecting..</pre> </span>
    
    <p style="text-align: center;">::<a href="https://github.com/easytarget/esp32-cjmcu-531-demo/" 
    title="Project home repo">GitHub</a>::</p>

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
          // If an angle field is present, display it.
          if (response.hasOwnProperty('Angle')) {
            document.getElementById("ANGLEValue").innerHTML = response.Angle;
          }        
        
          if (response.RangeStatus == 0) {
            plot = document.getElementById("plot").getContext("2d");
            var maxX = Math.floor(plot.canvas.width);
            var maxY = Math.floor(plot.canvas.height);
            plotvalue = Math.floor(response.Distance / plotscale);
            plotpoint = maxY - plotvalue;
            plot.fillStyle = "#DDDDDD";
            plot.fillRect(plotline, plotpoint, 1, plotvalue);
            plotline = plotline + 1;
            if (plotline > maxX) {
              plotline = 0;
            }
            plot.clearRect(plotline, 0, 1, maxY);
          }

          if (response.RangeStatus == 0) {
            scan = document.getElementById("scan").getContext("2d");
            var maxX = Math.floor(scan.canvas.width);
            var maxY = Math.floor(scan.canvas.height);
            scanvalue = Math.floor(response.Distance / plotscale);
            scanpoint = maxY - scanvalue;
            scanline = (response.Angle + 90) * (maxX / 181);
            scan.fillStyle = "#DDDDDD";
            scan.fillRect(scanline, scanpoint, 1, scanvalue);
            // scan.clearRect(scanline, 0, 1, maxY);
          }
        }
      }
      xhttp.open("GET", "/range", true);
      xhttp.send();
      xhttp.timeout = 300; // time in milliseconds
      xhttp.ontimeout = function () {
        document.getElementById("signal").style.color = "#8e0b0b";
        document.getElementById("RANGEValue").style.color = "#8e0b0b";
      };
    }
    
    // Read and process the extended info, handle mode changes
    var mode = "NONE";      // we dont know mode initially
    var havelidar = false;  // assume no lidar untill it is reported as present
    function getInfo() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          var response = JSON.parse(this.responseText);
          document.getElementById("statusText").innerHTML = this.responseText;
          if (response.Mode != mode) {  // mode changes
            mode = response.Mode;
            setMode(response.Mode);
          }
          if (response.HasServo == true) {  // lidar present?
            havelidar = true;
            showLidar();
          }
        }
      }
      xhttp.open("GET", "/info", true);
      xhttp.send();
      xhttp.timeout = 600; // time in milliseconds
    };

    function showPlot() {
      document.getElementById("plot").style.display = "block";
      document.getElementById("plotControl").style.display = "none";
    }
    
    function hidePlot() {
      document.getElementById("plot").style.display = "none";
      document.getElementById("plotControl").style.display = "block";
    }
  
    function showScan() {
      document.getElementById("scan").style.display = "block";
      document.getElementById("scanControl").style.display = "none";
    }
    
    function hideScan() {
      document.getElementById("scan").style.display = "none";
      document.getElementById("scanControl").style.display = "block";
    }
  
    function showControlPanel() {
      document.getElementById("controlPanel").style.display = "block";
      document.getElementById("statusPanel").style.display = "block";
      document.getElementById("showControl").style.display = "none";
    }

    function hideControlPanel() {
      document.getElementById("controlPanel").style.display = "none";
      document.getElementById("statusPanel").style.display = "none";
      document.getElementById("showControl").style.display = "block";
    }
   
    // Updates page to show the default-hidden lidar controls
    function showLidar() {
      var x = document.getElementsByClassName("lidar");
      var i;
      for (i = 0; i < x.length; i++) {
        x[i].style.display = "block";
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
        case "near":
          plotscale = 8;
          document.getElementById("MODEValue").innerHTML = "near (1.3m)";
          break;
        case "mid":
          plotscale = 16;
          document.getElementById("MODEValue").innerHTML = "mid (2.6m)";
          break;
        case "far":
          plotscale = 20;
          document.getElementById("MODEValue").innerHTML = "far (4m)";
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
