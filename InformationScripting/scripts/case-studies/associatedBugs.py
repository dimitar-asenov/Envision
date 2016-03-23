import re
from github3 import GitHub

gh = GitHub()
repo = gh.repository('dimitar-asenov', 'test-why-use-case')

def referencedIssues(commit):
    for issueId in re.findall('#(\d+)', commit):
        yield repo.issue(issueId)

for astMsg in Query.input.tuples('astMessage'):
    msg = '<b>Commit</b><br/>{}<br/>'.format(astMsg.message.replace('\n', '<br/>'))
    for issue in referencedIssues(astMsg.message):
        msg += '<b>Fixes #{}</b><br/>{}<br/>'.format(issue.number, issue.title)
    
    t = Tuple([('message', msg), ('ast', astMsg.ast), ('type', 'info')])
    Query.result.add(t)
