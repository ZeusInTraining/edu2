void CreateEDU1LaunchFile (float argc)
{
	float file;
	float i;
	
	file = fopen ("edu1launch.txt", FILE_WRITE);
	for (i = 1; i < argc; i++)
	{
		fputs (file, argv (i));
		fputs (file, "\n");
	}
	fclose (file);
}
