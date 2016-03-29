import re
from github3 import GitHub

gh = GitHub()
repo = gh.repository('dimitar-asenov', 'test-why-use-case')

def referencedIssues(commit):
	for issueId in re.findall('#(\d+)', commit):
		yield repo.issue(issueId)

for data in Query.input.tuples('data'):
	text = '<b>Commit</b><br/>{}<br/>'.format(
				data.message.replace('\n', '<br/>'))
	for issue in referencedIssues(data.message):
		text += '<b>Fixes #{}</b><br/>{}<br/>'.format(
						issue.number, issue.title)

	t = Tuple([ ('message', text),
			('ast', data.ast), ('type', 'info') ])
	Query.result.add(t)
