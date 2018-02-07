var link = ''
function download()
{
	if(link != '') {

		setInterval(function(){
	
			window.location.reload(true);		
		}, 2000);
					console.log("downloading")
			$('#modal1').modal('close');
			var a =	document.getElementById('download');
			a.href = link
			a.click(function(s){
				console.log(s);
			});
			link = '';
		
	}

}
	
function setLink(e)
{
	
	link = e
}

