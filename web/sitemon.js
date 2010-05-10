
function submitPostParams(url, params)
{
	var form = document.createElement('form');
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