---
layout: page
title: Downloads
permalink: /Downloads/
---


<div class="container" id="dl-container">
<a id="download" onClick="downloadclick()" href="" hidden ></a>

<div id="modal1" class="modal" style="width: 100% !important; max-height:100% !important">
  <div class="modal-content" style="height: 100%; padding: 0px; overflow: hidden"  >
    <iframe id="myModal" onLoad="download()" style="width:100%; height:100%" src="https://docs.google.com/forms/d/e/1FAIpQLSdw3voeoIFsAWOX9ao8TwBtS_cxG9A6IcImmdtgV0FSGXNnpA/viewform?..." frameborder="0" marginheight="0" marginwidth="0">Loading...</iframe>
  </div>


</div>


<div class="row">
    <div class="col s6">
      <span> <h4> <i class="zmdi zmdi-windows"> Windows </i></h4></span>
      <br>
      <table class="downloads" style>
        <thead>
          <tr>
            <th>Version</th>
            <th>Download</th>
          </tr>
        </thead>
        <tbody>
          <tr>
            <td style="padding-top: 2%; padding-bottom: 2%"><font color="#8985db">v0.2.1</font></td>
            <td style="padding-top: 2%; padding-bottom: 2%">
                  <a href='https://s3-us-west-2.amazonaws.com/elmaven-installers/Windows/El-Maven_v0.2.1_Windows_Installer.exe' style='display:hidden' id='win_0.2.1'></a>
                  <button style="background-color:#8985db;" onClick="setLink('win_0.2.1')" data-target="modal1" class="btn modal-trigger"><font color="white"><i class="zmdi zmdi-download"></i></font></button>

                
            </td>

          </tr>

          <tr>
            <td style="padding-top: 2%; padding-bottom: 2%"><font color="#8985db">v0.2.4</font></td>
            <td style="padding-top: 2%; padding-bottom: 2%">
                <a href='http://bit.ly/2BXfvzV' style='display:hidden' id='win_0.2.4'></a>
                <button style="background-color:#8985db;" onClick="setLink('win_0.2.4')" data-target="modal1" class="btn modal-trigger"><font color="white"><i class="zmdi zmdi-download"></i></font></button>
            </td>

          </tr>

        </tbody>
      </table>
    </div>
    <div class="col s6">
      <span> <h4> <i class="zmdi zmdi-apple"> Mac </i></h4></span>
      <br>
      <table class="downloads" style>
        <thead>
          <tr>
            <th>Version</th>
            <th>Download</th>
          </tr>
        </thead>
        <tbody>
          <tr>
            <td style="padding-top: 2%; padding-bottom: 2%"><font color="#8985db">v0.2.1</font></td>
            <td style="padding-top: 2%; padding-bottom: 2%">
            <a href='http://bit.ly/2EjKTxI' style='display:hidden' id='mac_0.2.1'></a>
                <button style="background-color:#8985db;" onClick="setLink('mac_0.2.1')" data-target="modal1" class="btn modal-trigger"><font color="white"><i class="zmdi zmdi-download"></i></font></button>
            </td>

          </tr>

          <tr>
            <td style="padding-top: 2%; padding-bottom: 2%"><font color="#8985db">v0.2.4</font></td>
            <td style="padding-top: 2%; padding-bottom: 2%">
            <a href='http://bit.ly/2E2ikFE' style='display:hidden' id='mac_0.2.4'></a>
                <button style="background-color:#8985db;" onClick="setLink('mac_0.2.4')" data-target="modal1" class="btn modal-trigger"><font color="white"><i class="zmdi zmdi-download"></i></font></button>
            </td>

          </tr>
        </tbody>
      </table>
    </div>
 </div>
</div>



