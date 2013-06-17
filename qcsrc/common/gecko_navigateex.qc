// Andreas Kirsch

// gecko navigation wrapper for offline mode
// use the gecko_offline_basepath to specify a directory (full absolute path) that contains an offline cache
// e.g. C:\webcache
// if you want to open www.google.com C:\webcache\www.google.com will be opened instead
void( string instanceName, string URL ) gecko_navigateex = {
	registercvar( "gecko_offline_basepath", "", 0 );
	string lOfflinePath;
	lOfflinePath = cvar_string( "gecko_offline_basepath" );
	string lURI;
	lURI = URL;
	if( lOfflinePath != "" ) {
		lOfflinePath = strreplace( "\\", "/", lOfflinePath );
		lURI = strreplace( "http:/", strcat( "file:///", lOfflinePath ), lURI ); // only one / because the other one is used as dir separator
		if( substring( lURI, strlen( lURI ) - 5, 5 ) != ".html" ) {
			lURI = strcat( lURI, "/index.html" );
		}
	}
	print( "NavigateEx to ", lURI, "\n" );
	gecko_navigate( instanceName, lURI );
}
