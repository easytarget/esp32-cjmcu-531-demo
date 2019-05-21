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
    height: 20px;
    background-color: #666;
    border: none;
    color: #FFF;
    padding: none ;
    text-align: center;
    text-decoration: none;
    display: block;
    margin-left: auto;
    margin-right: auto;
    margin-top: 3px;
    margin-bottom: 6px;
    cursor: zoom-in;
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

<!-- It's meta, baby -->
<head>
  <title>ESP32/VL53L0X display and control demo</title>
  <meta charset="utf-8" />
  <meta name="viewport" content="width=400, initial-scale=1" />
  <meta http-equiv="Cache-control" content="no-cache">
</head>

<!-- The page itself -->
<body>
  <div class="card">
    <h3 style="text-align: center;">ESP32/VL53L0X Demo
    <span id="signal" class="signal">Timeout</span></h3>
    
    <hr>
    
    <h2 style="text-align: center;">Range (mm):</h2>
    <h1 style="text-align: center;"><span id="RANGEValue" style="color: #056016">
    Connecting..</span></h1>

    <div class="plot" id="plot" onclick="togglePlot()">Evil Plot</div>
    
    <hr>
    
     <div style="text-align: center;">
      Sensitivity&nbsp;&nbsp;::&nbsp;&nbsp;
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

    <div style="text-align: center;">
      Sensor&nbsp;&nbsp;::&nbsp;&nbsp;
      <button class="button" onclick="httpGet('/on')" 
              title="Enable sensor">On</button>
      &nbsp;&nbsp;||&nbsp;&nbsp;
      <button class="button" onclick="httpGet('/off')" 
              title="Disable sensor">Off</button>
    </div>

    <span onclick="toggleStatusPanel()">
      <h3 style="text-align: center; cursor: pointer; text-decoration: underline;">
      Show Status</h3>
      <pre><span id="INFOValue" style="color: #056016; display: none;">
      Connecting..</span></pre>
    </span>
    <p style="text-align: right;">::<a href="https://easytarget.org/">easytarget.org</a>::
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
        }
        //if (response.Angle) {
        //  document.getElementById("RANGEValue").innerHTML += response.Angle;
        //}
      };
      xhttp.open("GET", "/range", true);
      xhttp.send();
      xhttp.timeout = 300; // time in milliseconds
      xhttp.ontimeout = function () {
        document.getElementById("signal").style.color = "#8e0b0b";
      };
    }
    
    function getInfo() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          var response = this.responseText;
          document.getElementById("INFOValue").innerHTML = response;
       }
      };
      xhttp.open("GET", "/info", true);
      xhttp.send();
      xhttp.timeout = 600; // time in milliseconds
    };

    // Updates page to show the default-hidden lidar controls
    function enableLidar() {
      var x = document.getElementsByClassName("lidar");
      var i;
      for (i = 0; i < x.length; i++) {
        x[i].style.display = block;
      }
    }

    var plot = false;
    function togglePlot() {
      if (plot) { //collapse
        document.getElementById("plot").innerHTML = "Totally Evil Plot!";
        document.getElementById("plot").style.height = "20px";
        document.getElementById("plot").style.cursor = "zoom-in";
        plot = false;
      } else { // expand
        document.getElementById("plot").innerHTML = "HELLO"; 
        document.getElementById("plot").style.height = "240px";
        document.getElementById("plot").style.cursor = "zoom-out";
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
  </script>
</body>
</html>
)=====";
