const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>

<style>
.plot{
  width: 400px;
  height: 256px;
  background-color: #222;
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
  cursor: crosshair;
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
  max-width: 450px;
  min-height: 600px;
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
</style>

<head>
  <title>ESP32/VL53L0X display and control demo</title>
  <meta charset="utf-8" />
  <meta name="viewport" content="width=400, initial-scale=1" />
  <meta http-equiv="Cache-control" content="no-cache">
</head>

<body>

  <div class="card">
    <h3 style="text-align: center;">ESP32/VL53L0X Demo</h3>
    <!--<hr style="background: #056016; border: 1px;">-->
    <hr>
    <h1>Range (mm): <span id="RANGEValue" style="color: #056016">Connecting</span></h1>
    <h2>Status: <span id="INFOValue" style="color: #056016">Unset</span></h2>
    <div class="plot">Plot Goes Here</div>
    <hr>
     <div style="text-align: center;">
      Sensitivity&nbsp;&nbsp;::&nbsp;&nbsp;
      <button class="button" onclick="httpGet('/near')">Near</button>
      &nbsp;&nbsp;||&nbsp;&nbsp;
      <button class="button" onclick="httpGet('/mid')">Mid</button>
      &nbsp;&nbsp;||&nbsp;&nbsp;
      <button class="button" onclick="httpGet('/far')">Far</button>
    </div>
    <br>
    <div style="text-align: center;">
      ROI&nbsp;&nbsp;::&nbsp;&nbsp;
      <button class="button" onclick="httpGet('/roiminus')">Smaller</button>
      &nbsp;&nbsp;||&nbsp;&nbsp;
      <button class="button" onclick="httpGet('/roiplus')">Bigger</button>
    </div>
    <br>
    <div style="text-align: center;">
      Rotation&nbsp;&nbsp;::&nbsp;&nbsp;
      <button class="button" onclick="httpGet('/left')">left</button>
      &nbsp;&nbsp;||&nbsp;&nbsp;
      <button class="button" onclick="httpGet('/right')">Right</button>
    </div>
    <br>
    <div style="text-align: center;">
      Sensor&nbsp;&nbsp;::&nbsp;&nbsp;
      <button class="button" onclick="httpGet('/on')">On</button>
      &nbsp;&nbsp;||&nbsp;&nbsp;
      <button class="button" onclick="httpGet('/off')">Off</button>
    </div>
    <hr>
    <p style="text-align: right;">Brought to you by: <a href="https://easytarget.org/">easytarget.org</a>
  </div>
<script>

setInterval(function() {
  // Call a function repetatively with 0.2 Second interval
  getData();
}, 200); // 5 times per second

setInterval(function() {
  // Call a function repetatively with 0.2 Second interval
  getInfo();
}, 333); // thrice per second

function httpGet(theUrl)
{
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.open( "GET", theUrl, false ); // false for synchronous request
    xmlHttp.send( null );
    return xmlHttp.responseText;
}

function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("RANGEValue").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readRANGE", true);
  xhttp.send();
}

function getInfo() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("INFOValue").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "/info", true);
  xhttp.send();
}

</script>
</body>
</html>
)=====";
