
function submitPostParams(url, params)
{
	var form = document.createElement('form');
	document.body.appendChild(form);
	form.action = url;
	form.method = 'POST';
	
	for (var i in params)
	{
		if (params.hasOwnProperty(i))
		{
			var input = document.createElement('input');
			input.type = 'hidden';
			input.name = i;
			input.value = params[i];
			
			form.appendChild(input);
		}
	}
	
	form.submit();
}

function submitAjaxPostParams(url, params)
{
	if (window.XMLHttpRequest)
	{
		xmlhttp = new XMLHttpRequest();
		xmlhttp.open("POST", url, true);
		
		var tempParamString = "";
		for (var i in params)
		{
			tempParamString += i + "=" + params[i] + "&";
		}
		
		var paramString = tempParamString.slice(0, -1);
		
		xmlhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
		xmlhttp.setRequestHeader("Content-length", paramString.length);
		xmlhttp.setRequestHeader("Connection", "close");
		xmlhttp.send(paramString);
	}
}

function deleteSingleTest(testID)
{
	if (confirm('Are you sure you want to delete this single test?'))
	{
		submitPostParams('/delete_single_test', {'test_id' : testID});
	}
}

function deleteScriptTest(testID)
{
	if (confirm('Are you sure you want to delete this script test?'))
	{
		submitPostParams('/delete_script_test', {'test_id' : testID});
	}
}

function deleteScriptStep(testID, pageID)
{
	if (confirm('Are you sure you want to delete this script step?'))
	{
		submitPostParams('/delete_script_step', {'test_id' : testID, 'page_id' : pageID});
	}
}

function runManualSingleTest(testID)
{
	submitAjaxPostParams('/run_man_single', {'test_id' : testID});
}

function runManualScriptTest(testID)
{
	submitAjaxPostParams('/run_man_script', {'test_id' : testID});
}
