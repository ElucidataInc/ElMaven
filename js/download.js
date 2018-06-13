var link = ''
var downloadStart = 0;
var current_id=''
var frame_id="";
var div_id="";
function download(frame_id,div_id)
{	
	if(++downloadStart%2==0) {

					console.log("downloading")
			$('#'+div_id).modal('close'); //div id
			var a =	document.getElementById(current_id);
			console.log(a)
			a.click(function(s){
				console.log(s);
			});
			link = '';
			document.getElementById(frame_id).src=document.getElementById(frame_id).src //frame id
		
	}

}

function downloadclick() {
	downloadStart = true;
}

function reload() {
	if(downloadStart) {
		setInterval(function(){
	
			window.location.reload(true);		
		}, 2000);
	}

}

	
function setLink(e)
{	console.log(e)
	current_id=e
}

