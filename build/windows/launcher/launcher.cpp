// Imager.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "launcher.h"

// The size of all of the strings was made sort of ambiguously large, since
// 1) nothing is hurt by allocating an extra few bytes temporarily and
// 2) if the user has a long path, and it gets copied five times over for the
// classpath, the program runs the risk of crashing. Bad bad.

#define STACKSIZE_ARGS "-mx60m -ms60m "
#define STACKSIZE_MATCH " -mx"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImagerApp

BEGIN_MESSAGE_MAP(CImagerApp, CWinApp)
	//{{AFX_MSG_MAP(CImagerApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImagerApp construction

CImagerApp::CImagerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CImagerApp object

CImagerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CImagerApp initialization

BOOL CImagerApp::InitInstance()
{
	// all these malloc statements... things may need to be larger.

	// what was passed to this application
	char *incoming_cmdline = (char *)malloc(256 * sizeof(char));
	strcpy (incoming_cmdline, this->m_lpCmdLine);
	
	// what gets put together to pass to jre
	char *outgoing_cmdline = (char *)malloc(2048 * sizeof(char));
	char *p = outgoing_cmdline;
	
	// prepend the args for -mx and -ms if they weren't
	// specified on the command line by the user
	if (strstr(incoming_cmdline, STACKSIZE_MATCH)) {
		// need to split stack args and documents
		while (true) {
			char c = *incoming_cmdline++;
			if (c == ' ') {
				if (*incoming_cmdline != '-') {
					break;
				} else {
					*p++ = ' ';
				}
			} else if (c == 0) {
				incoming_cmdline--;
				*p++ = ' ';
				break;
			} else {
				*p++ = c;
			}
		}
		*p++ = 0;
	} else {
		strcpy(outgoing_cmdline, STACKSIZE_ARGS);
	}

	// append the classpath and imager.Application
	char *cp = (char *)malloc(1024 * sizeof(char));
	char *loaddir = (char *)malloc(MAX_PATH * sizeof(char));

    GetModuleFileName(NULL, loaddir, MAX_PATH);
    // remove the application name
	*(strrchr(loaddir, '\\')) = '\0';

    //cp = (char *)malloc(5 * strlen(loaddir) + 200);
	// put quotes around contents of cp, 
	// because %s might have spaces in it.
    sprintf(cp, 
		"-cp \""
        "%s\\lib\\imager.jar;"
	    "%s\\lib\\swing.zip;"
        "%s\\lib\\rt.jar;"
	    "%s\\lib\\i18n.jar;"
	    "%s\\lib\\classes.zip"
		"\" ",
	    loaddir, loaddir, loaddir, loaddir, loaddir);
	//buildClassPath(cp);
	strcat(outgoing_cmdline, cp);

	// add the name of the class to execute
	strcat(outgoing_cmdline, "imager.Application ");

	// append additional incoming stuff (document names), if any
	strcat(outgoing_cmdline, incoming_cmdline);

	//AfxMessageBox(outgoing_cmdline);

	char *executable = (char *)malloc(256 * sizeof(char));
	// loaddir is the name path to the current application
	strcpy(executable, loaddir);
	// copy in the path for jrew, relative to imager.exe
	strcat(executable, "\\bin\\jrew");
	
	//AfxMessageBox(executable);

	char *path = (char *)malloc(1024 * sizeof(char));
	char *old_path = (char *)malloc(1024 * sizeof(char));
	strcpy(old_path, getenv("PATH"));
	// need to add the lib directory to the path, because it contains the
	// dll for the dongle, and eventually will also contain magician
	strcpy(path, "PATH=");
	strcat(path, old_path);
	strcat(path, ";");
	strcat(path, loaddir);
	strcat(path, "\\lib");
	//AfxMessageBox(path);
	putenv(path);

	HINSTANCE result;
	result = ShellExecute(NULL, "open", executable,
		                  outgoing_cmdline, NULL, SW_SHOWNORMAL);

	if ((int)result <= 32) {
		// some type of error occurred
		switch ((int)result) {
			case ERROR_FILE_NOT_FOUND:
			case ERROR_PATH_NOT_FOUND:
				AfxMessageBox("A required file could not be found, please re-install Imager.");
				break;
			case 0:
			case SE_ERR_OOM:
				AfxMessageBox("Not enough memory or resources to run Imager at this time.");
				break;
			default:
				AfxMessageBox("There is a problem with your Imager installation.\n"
					          "If the problem persists, re-install Imager.");
				break;
		}
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return TRUE;
}
