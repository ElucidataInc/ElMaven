var link = ''
var downloadStart = 0;
var current_id=''
function download()
{	
	if(++downloadStart%2==0) {

					console.log("downloading")
			$('#modal1').modal('close');
			var a =	document.getElementById(current_id);
			console.log(a)
			a.click(function(s){
				console.log(s);
			});
			link = '';
			document.getElementById("myModal").src=document.getElementById("myModal").src
		
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

