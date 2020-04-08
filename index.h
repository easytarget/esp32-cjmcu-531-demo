const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>

<!-- Page served by ESP32 CJMCU-531 demo; see https://github.com/easytarget/esp32-cjmcu-531-demo/ 
     Design/Coding by Owen; https://easytarget.org/ -->

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
    margin: 3px 4px;
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
    color: #056016; 
    
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
  .signal{
    color: #8e0b0b;
    text-align: right;
  }
</style>

<!-- Top -->
<head>
  <title>ESP32/VL53L1X display and control demo</title>
  <meta charset="utf-8" />
  <meta name="viewport" content="width=380, initial-scale=1" />
  <meta http-equiv="Cache-control" content="no-cache">
</head>

<!-- The page itself -->
<body>
  <div class="card">
    <h3 style="text-align: center;">ESP32/VL53L1X Demo</h3>
    <hr> 
    <div id="signal" class="signal">Comms Timeout</div>
    <div><span id="RANGEValue" style="float: right; font-size: 200%;font-weight: bold;">connecting..</span>
      <span style="float: left; font-size: 160%;font-weight: bold;">Range:</span></div><hr>
    <div><span id="MODEValue"style="float: right; font-weight: bold;">unknown</span>&nbsp;
      <span style="float: left;font-weight: bold;">Mode:</span></div>
    <div class="lidar"><span id="ANGLEValue" style="float: right; font-weight: bold;">
      Unknown</span>
      <span style="float: left; font-weight: bold;">Lidar Angle:</span></div><hr>
      
    <span class="lidar"><canvas class="canvases" id="scan" onclick="hideScan()" width=320 height=240>
    This is a Canvas Element, if it is not displayed then we apologize, your browser 
    is not compatible.</canvas>
    <div class="expander" id="scanControl" onclick="showScan()"style="font-size: 110%; font-weight: bold;">show scan</div>
    <div class="expander" id="scanClear" onclick="clearScan()" style="font-size: 90%; display: none;">
    clear scan</div></span>
    <canvas class="canvases" id="plot" onclick="hidePlot()" width=320 height=240>
    This is a Canvas Element, if it is not displayed then we apologize, your browser 
    is not compatible.</canvas>
    <div class="expander" id="plotControl" onclick="showPlot()" style="font-size: 110%; font-weight: bold;">show history</div>

    <hr>
    <div style="text-align: left;">
      Sensor&nbsp;::<button class="button" onclick="httpGet('/on')"
              title="Enable sensor">On</button>|<button class="button" onclick="httpGet('/off')"
              title="Disable sensor">Off</button>
    </div>
    <div style="text-align: left;">
      Mode&nbsp;::<button class="button" onclick="httpGet('/near')" 
              title="Near range (max 1.3m, fastest)">Near</button>|<button class="button" onclick="httpGet('/mid')" 
              title="Mid Range (max 4m, fast but less accurate at range)">Mid</button>|<button class="button" onclick="httpGet('/far')" 
              title="Far Range (max 4m, slower but more accurate over whole range)">Far</button>
    </div>
    <div style="text-align: left;" class="lidar">
      Scan&nbsp;::<button class="button" onclick="httpGet('/s-scan90');showScan()" 
              title="scan 90 degrees around current direction">90&deg;</button>|<button class="button" onclick="httpGet('/s-scanfull');showScan()"
              title="Scan 270 degrees">270&deg;</button>|<button class="button" onclick="httpGet('/s-scanstop')" 
              title="Stop scanning">Stop</button>|<button class="button" onclick="httpGet('/s-scanback')" 
              title="Reverse scanning">Back</button>
    </div>
    <div style="text-align: left;" class="lidar">
      Motion&nbsp;::<button class="button" onclick="httpGet('/s-left')" 
              title="Swing the sensor left">Left</button>|<button class="button" onclick="httpGet('/s-home')"
              title="Go to the Home (zero) position and turn stepper off">Home</button>|<button class="button" onclick="httpGet('/s-right')" 
              title="Swing the sensor right">Right</button>
    </div>
    <div>
      <h3 onclick="showControlPanel()" id="showControl" 
        class="expander">
      more control</h3>
      <span id="controlPanel" style="display: none;">
        <h3 onclick="hideControlPanel()" class="expander">
          less control
        </h3>
        <div style="text-align: left;">
          TimingBudget&nbsp;::<button class="button" onclick="httpGet('/budgetminus')" 
             title="Make Timing Budget smaller">Smaller</button>|<button class="button" onclick="httpGet('/budgetplus')" 
             title="Make Timing Budget bigger">Bigger</button>
        </div>
        <div style="text-align: left;">
          Interval&nbsp;::<button class="button" onclick="httpGet('/intervalminus')" 
             title="Make InterMeasurement period smaller">Smaller</button>|<button class="button" onclick="httpGet('/intervalplus')" 
             title="Make InterMeasurement period bigger">Bigger</button>
        </div>
        <div style="text-align: left;">
          ROI&nbsp;::<button class="button" onclick="httpGet('/roiminus')" 
             title="Make Region of Interest smaller">Smaller</button>|<button class="button" onclick="httpGet('/roiplus')" 
             title="Make Region of Interest bigger">Bigger</button>
        </div>
        <div style="text-align: left;" class="lidar">
          Scan Step&nbsp;::<button class="button" onclick="httpGet('/s-scanminus')" 
             title="Make scan steps smaller">Smaller</button>|<button class="button" onclick="httpGet('/s-scanplus')" 
             title="Make scan steps bigger">Bigger</button>
        </div>
        <div style="text-align: left;" class="lidar">
          Manual Step&nbsp;::<button class="button" onclick="httpGet('/s-manualminus')" 
             title="Make manual steps smaller">Smaller</button>|<button class="button" onclick="httpGet('/s-manualplus')" 
             title="Make manual steps bigger">Bigger</button>
        </div>
        <div style="text-align: left;" class="lidar">
          Step Control&nbsp;::<button class="button" onclick="httpGet('/s-off')" 
             title="Power Down the stepper">Off</button>|
          <button class="button" onclick="httpGet('/s-zero')" 
             title="Declare new Zero position">Zero</button>
        </div>
      </span>
    </div>
    <span id="statusPanel" class="expander" style="display: none; text-align: left; cursor: auto;">
    <pre style="font-size: 110%; font-weight: bold;">Status::</pre><pre id="statusText" style="font-weight: normal;">
    Connecting..</pre></span>
    <div style="text-align: center;">::<a href="https://github.com/easytarget/esp32-cjmcu-531-demo/" 
    title="Project home repo">GitHub</a>::</div>
  </div>

  <!-- The scripting -->
  
  <script>

    // Master interval (update rate) setting.
    updateInterval = 250;
    minUpdateInterval = 100; // fastest we can go, even at higher sensor speeds
  
    // Create the recurring interval task to refresh the reading data 
    setInterval(function() {
      getData();
    }, updateInterval); // interval in ms. -- 5 reading requests/second

    // make a simple http request and return result. Used to trigger actions from buttons
    function httpGet(theUrl)
    {
        var xmlHttp = new XMLHttpRequest();
        xmlHttp.open( "GET", theUrl, true); // asynchronous, and we dont care about return.
        xmlHttp.send( null );
    }

    // Loop to get, process and display value readings
    var plotline = 0;       // history plot position
    var plotscale = 20;     // scale factor for the simple plots
    var timeout = 0;        // counts reading timeouts/failures
    var mode = "NONE";      // we dont know mode initially
    var haveLidar = false;  // assume no lidar untill it is reported as present
    
    function getData() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          var response = JSON.parse(this.responseText);

          document.getElementById("statusText").innerHTML = this.responseText;
          document.getElementById("signal").innerHTML = "&nbsp;"; //some content is needed to prevent element collapsing
          if (response.RangeStatus < 0) {
            timeout = 0; // dont timeout while disabled
            document.getElementById("RANGEValue").innerHTML = "<span style=\"font-size: 80%;\">Disabled</span>";
            document.getElementById("RANGEValue").style.color = "#555";
          } else {
            timeout++; // always increment the timeout, it will only be reset when we have a valid range
            switch (response.RangeStatus) {
                case 0:
                  timeout = 0; // valid range result, reset timeout counter
                  document.getElementById("RANGEValue").innerHTML = response.Distance;
                  document.getElementById("RANGEValue").innerHTML += "<span style=\"font-size: 66%;\">mm</span>";
                  document.getElementById("RANGEValue").style.color = "#555";
                break;
                case 1:
                  document.getElementById("signal").innerHTML = "Sigma Fail";
                  document.getElementById("RANGEValue").style.color = "#8e0b0b";
                break;
                case 2:
                  document.getElementById("signal").innerHTML = "Signal fail";
                  document.getElementById("RANGEValue").style.color = "#8e0b0b";
                break;
                case 4:
                  document.getElementById("signal").innerHTML = "Out of Bounds";
                  document.getElementById("RANGEValue").style.color = "#8e0b0b";
                break;
                case 5:
                  document.getElementById("signal").innerHTML = "Hardware Fail";
                  document.getElementById("RANGEValue").style.color = "#8e0b0b";
                break;
                case 7:
                  document.getElementById("signal").innerHTML = "Wrapped target";
                  document.getElementById("RANGEValue").style.color = "#8e0b0b";
                break;
                case 8:
                  document.getElementById("signal").innerHTML = "Internal Error";
                  document.getElementById("RANGEValue").style.color = "#8e0b0b";
                break;
                case 14:
                  document.getElementById("signal").innerHTML = "Invalid Result";
                  document.getElementById("RANGEValue").style.color = "#8e0b0b";
                break;
                case 255:
                  document.getElementById("signal").innerHTML = "No Response";
                  document.getElementById("RANGEValue").style.color = "#8e0b0b";
                break;
                default:
                  document.getElementById("signal").innerHTML = "Unknown Error";
                  document.getElementById("RANGEValue").style.color = "#8e0b0b;";
            }
            // if timeout reached, clear the range value.
            if (timeout > 10) document.getElementById("RANGEValue").innerHTML = "n/a";
          }
          
          // If an angle field is present, display it.
          if (response.hasOwnProperty('Angle')) {
            document.getElementById("ANGLEValue").innerHTML = response.Angle;
            document.getElementById("ANGLEValue").innerHTML += "&deg;";
            if (response.ScanControl != 0) document.getElementById("ANGLEValue").innerHTML += " (scanning)";
          }

          // Detect and respond to mode changes
          if (response.Mode != mode) {
            mode = response.Mode;
            setMode(response.Mode);
          }

          // Adjust update interval based on Intermeasurement period (sensor reading speed).
          var lastInterval = response.IntermeasurementPeriod + 50;
          if (lastInterval > minUpdateInterval) {
            updateInterval = lastInterval;
          }
           

          // Detect and enable stepper(lidar) functions
          if ((response.HasServo == true) && !haveLidar) {  // stepper present but undetected?
            haveLidar = true;
            showLidar();
          }

          // Plot the history histogram if we got a valid response
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

          // Plot the scan if enabled and we have a valid response
          if ((response.RangeStatus == 0) && haveLidar) {
            scan = document.getElementById("scan").getContext("2d");
              var ScanXYRadius = Math.floor(scan.canvas.width/2); // radius= 1/2 the plot width
              var rad = response.Angle * Math.PI / 180;
              var scanValue = Math.floor(response.Distance / plotscale);
              var scanX = Math.floor(Math.sin(rad)*scanValue);
              var scanY = Math.floor(Math.cos(rad)*scanValue);
              scan.fillStyle = "#DDDDDD";
              scan.fillRect(ScanXYRadius+scanX, ScanXYRadius-scanY, 3, 3);
           }
        }
      }
      xhttp.open("GET", "/data", true);
      xhttp.send();
      xhttp.timeout = 300; // time in milliseconds
      xhttp.ontimeout = function () {
        timeout++; // increment timeout counter
        // if 4xtimeout reached without any valid replies, clear the range value
        if (timeout > 4) {
          document.getElementById("RANGEValue").innerHTML = "n/a";
          document.getElementById("signal").innerHTML = "Network Timeout";
        };
      };
    }

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
      document.getElementById("scanClear").style.display = "block";
    }
    
    function hideScan() {
      document.getElementById("scan").style.display = "none";
      document.getElementById("scanControl").style.display = "block";
      document.getElementById("scanClear").style.display = "none";
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
   
    // Updates page to show the lidar controls when present
    function showLidar() {
      var x = document.getElementsByClassName("lidar");
      var i;
      for (i = 0; i < x.length; i++) {
        x[i].style.display = "block";
        }
    }

    // Called when a mode change is detected; sets the graph etc.
    function setMode(mode) {
      clearPlot();
      clearScan();
      switch (mode) {
        case "near":
          plotscale = 5;
          document.getElementById("MODEValue").innerHTML = "near (1.3m)";
          break;
        case "mid":
          plotscale = 12;
          document.getElementById("MODEValue").innerHTML = "mid (2.6m)";
          break;
        case "far":
          plotscale = 20;
          document.getElementById("MODEValue").innerHTML = "far (4m)";
          break;
        default:
          plotscale = 16;
          document.getElementById("MODEValue").innerHTML = "Unknown";
      }
    }

    // Helper Functions for the canvas plots
    function clearPlot() {
      plot = document.getElementById("plot").getContext("2d");
      var maxPlotX = Math.floor(plot.canvas.width);
      var maxPlotY = Math.floor(plot.canvas.height);
      plot.clearRect(0, 0, maxPlotX, maxPlotY);
      plotline = 0;
    }

    function clearScan() {
      scan = document.getElementById("scan").getContext("2d");
      var maxScanX = Math.floor(scan.canvas.width);
      var maxScanY = Math.floor(scan.canvas.height);
      scan.clearRect(0, 0, maxScanX, maxScanY);
      // Add a reference 'dot' at the scan center
      var ScanXYRadius = Math.floor(scan.canvas.width/2); // radius= 1/2 the plot width
      scan.fillStyle = "#000000";
      scan.fillRect(ScanXYRadius-1,ScanXYRadius-1, 3, 3);
    }

  </script>
</body>
</html>
)=====";
