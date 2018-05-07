---
layout: page
title: Downloads
permalink: /Downloads/
---

<div class="container" id="dl-container">
    <a id="download" onClick="downloadclick()" href="" hidden></a>
    <div id="modal1" class="modal" style="width: 100% !important; max-height:100% !important">
        <div class="modal-content" style="height: 100%; padding: 0px; overflow: hidden">
            <iframe id="myModal" onLoad="download()" style="width:100%; height:100%" src="https://docs.google.com/forms/d/e/1FAIpQLSdw3voeoIFsAWOX9ao8TwBtS_cxG9A6IcImmdtgV0FSGXNnpA/viewform?..."
                frameborder="0" marginheight="0" marginwidth="0">Loading...</iframe>
        </div>
    </div>
    <div class="row">
        <div class="col s6">
            <span>
                <h4>
                    <i class="zmdi zmdi-windows"> Windows </i>
                </h4>
            </span>
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
                        <td style="padding-top: 2%; padding-bottom: 2%">
                            <font color="#8985db">v0.3.1 (beta)</font>
                        </td>
                        <td style="padding-top: 2%; padding-bottom: 2%">
                            <a href='https://github.com/ElucidataInc/ElMaven/releases/download/v0.3.1/Windows-El-Maven-v0.3.1.exe' style='display:hidden' id='win_0.3.1'></a>
                            <button style="background-color:#8985db;" onClick="setLink('win_0.3.1')" data-target="modal1" class="btn modal-trigger">
                                <font color="white">
                                    <i class="zmdi zmdi-download"></i>
                                </font>
                            </button>
                        </td>
                    </tr>
                    <tr>
                        <td style="padding-top: 2%; padding-bottom: 2%">
                            <font color="#8985db">v0.2.4</font>
                        </td>
                        <td style="padding-top: 2%; padding-bottom: 2%">
                            <a href='https://www.dropbox.com/s/vbc68cln9h7xyc6/El-Maven-v0.2.4.exe?dl=1' style='display:hidden' id='win_0.2.4'></a>
                            <button style="background-color:#8985db;" onClick="setLink('win_0.2.4')" data-target="modal1" class="btn modal-trigger">
                                <font color="white">
                                    <i class="zmdi zmdi-download"></i>
                                </font>
                            </button>
                        </td>
                    </tr>
                    <tr>
                        <td style="padding-top: 2%; padding-bottom: 2%">
                            <font color="#8985db">v0.2.1</font>
                        </td>
                        <td style="padding-top: 2%; padding-bottom: 2%">
                            <a href='https://s3-us-west-2.amazonaws.com/elmaven-installers/Windows/El-Maven_v0.2.1_Windows_Installer.exe' style='display:hidden'
                                id='win_0.2.1'></a>
                            <button style="background-color:#8985db;" onClick="setLink('win_0.2.1')" data-target="modal1" class="btn modal-trigger">
                                <font color="white">
                                    <i class="zmdi zmdi-download"></i>
                                </font>
                            </button>
                        </td>
                    </tr>
                </tbody>
            </table>
        </div>
        <div class="col s6">
            <span>
                <h4>
                    <i class="zmdi zmdi-apple"> Mac </i>
                </h4>
            </span>
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
                        <td style="padding-top: 2%; padding-bottom: 2%">
                            <font color="#8985db">v0.3.1</font>
                        </td>
                        <td style="padding-top: 2%; padding-bottom: 2%">
                            <a href='https://github.com/ElucidataInc/ElMaven/releases/download/v0.3.1/Mac-El-Maven-v0.3.1.app.zip' style='display:hidden' id='mac_0.3.1'></a>
                            <button style="background-color:#8985db;" onClick="setLink('mac_0.3.1')" data-target="modal1" class="btn modal-trigger">
                                <font color="white">
                                    <i class="zmdi zmdi-download"></i>
                                </font>
                            </button>
                        </td>
                    </tr>
                    <tr>
                        <td style="padding-top: 2%; padding-bottom: 2%">
                            <font color="#8985db">v0.2.4</font>
                        </td>
                        <td style="padding-top: 2%; padding-bottom: 2%">
                            <a href='https://www.dropbox.com/s/zdqfjhx0f678z2p/El-Maven-v0.2.4.zip?dl=1' style='display:hidden' id='mac_0.2.4'></a>
                            <button style="background-color:#8985db;" onClick="setLink('mac_0.2.4')" data-target="modal1" class="btn modal-trigger">
                                <font color="white">
                                    <i class="zmdi zmdi-download"></i>
                                </font>
                            </button>
                        </td>
                    </tr>
                    <tr>
                        <td style="padding-top: 2%; padding-bottom: 2%">
                            <font color="#8985db">v0.2.1</font>
                        </td>
                        <td style="padding-top: 2%; padding-bottom: 2%">
                            <a href='http://bit.ly/2EjKTxI' style='display:hidden' id='mac_0.2.1'></a>
                            <button style="background-color:#8985db;" onClick="setLink('mac_0.2.1')" data-target="modal1" class="btn modal-trigger">
                                <font color="white">
                                    <i class="zmdi zmdi-download"></i>
                                </font>
                            </button>
                        </td>
                    </tr>
                </tbody>
            </table>
        </div>
    </div>
<br />
            <div class="col-6">
                <h4>
                    <font color="">
                        <i class="zmdi zmdi-download">
                        Demo Datasets:</i>
                    </font>
                </h4>
                <br />
                <a style="color:#8985db" class="download-files" href="http://genomics-pubs.princeton.edu/mzroll/datasets/exampleProject.zip">Download example dataset</a>
                <br />
            </div>
</div>
