string wordwrap_buffer;

void wordwrap_buffer_put(string s)
{
	wordwrap_buffer = strcat(wordwrap_buffer, s);
}

string wordwrap(string s, float l)
{
	string r;
	wordwrap_buffer = "";
	wordwrap_cb(s, l, wordwrap_buffer_put);
	r = wordwrap_buffer;
	wordwrap_buffer = "";
	return r;
}

#ifndef MENUQC
void wordwrap_buffer_sprint(string s)
{
	wordwrap_buffer = strcat(wordwrap_buffer, s);
	if(s == "\n")
	{
		sprint(self, wordwrap_buffer);
		wordwrap_buffer = "";
	}
}

void wordwrap_sprint(string s, float l)
{
	wordwrap_buffer = "";
	wordwrap_cb(s, l, wordwrap_buffer_sprint);
	if(wordwrap_buffer != "")
		sprint(self, strcat(wordwrap_buffer, "\n"));
	wordwrap_buffer = "";
	return;
}
#endif

void wordwrap_cb(string s, float l, void(string) callback)
{
	local string c;
	local float lleft, i, j, wlen;

	s = strzone(s);
	lleft = l;
	for (i = 0;i < strlen(s);i++)
	{
		if (substring(s, i, 2) == "\\n")
		{
			callback("\n");
			lleft = l;
			i++;
		}
		else if (substring(s, i, 1) == "\n")
		{
			callback("\n");
			lleft = l;
		}
		else if (substring(s, i, 1) == " ")
		{
			if (lleft > 0)
			{
				callback(" ");
				lleft = lleft - 1;
			}
		}
		else
		{
			for (j = i+1;j < strlen(s);j++)
				//    ^^ this skips over the first character of a word, which
				//       is ALWAYS part of the word
				//       this is safe since if i+1 == strlen(s), i will become
				//       strlen(s)-1 at the end of this block and the function
				//       will terminate. A space can't be the first character we
				//       read here, and neither can a \n be the start, since these
				//       two cases have been handled above.
			{
				c = substring(s, j, 1);
				if (c == " ")
					break;
				if (c == "\\")
					break;
				if (c == "\n")
					break;
				// we need to keep this tempstring alive even if substring is
				// called repeatedly, so call strcat even though we're not
				// doing anything
				callback("");
			}
			wlen = j - i;
			if (lleft < wlen)
			{
				callback("\n");
				lleft = l;
			}
			callback(substring(s, i, wlen));
			lleft = lleft - wlen;
			i = j - 1;
		}
	}
	strunzone(s);
}

float dist_point_line(vector p, vector l0, vector ldir)
{
	ldir = normalize(ldir);

	// remove the component in line direction
	p = p - (p * ldir) * ldir;

	// vlen of the remaining vector
	return vlen(p);
}

void depthfirst(entity start, .entity up, .entity downleft, .entity right, void(entity, entity) funcPre, void(entity, entity) funcPost, entity pass)
{
	entity e;
	e = start;
	funcPre(pass, e);
	while(e.downleft)
	{
		e = e.downleft;
		funcPre(pass, e);
	}
	funcPost(pass, e);
	while(e != start)
	{
		if(e.right)
		{
			e = e.right;
			funcPre(pass, e);
			while(e.downleft)
			{
				e = e.downleft;
				funcPre(pass, e);
			}
		}
		else
			e = e.up;
		funcPost(pass, e);
	}
}

float median(float a, float b, float c)
{
	if(a < c)
		return bound(a, b, c);
	return bound(c, b, a);
}

// converts a number to a string with the indicated number of decimals
// works for up to 10 decimals!
string ftos_decimals(float number, float decimals)
{
	string result;
	string tmp;
	float len;

	// if negative, cut off the sign first
	if(number < 0)
		return strcat("-", ftos_decimals(-number, decimals));
	// it now is always positive!

	// 3.516 -> 352
	number = floor(number * pow(10, decimals) + 0.5);

	// 352 -> "352"
	result = ftos(number);
	len = strlen(result);
	// does it have a decimal point (should not happen)? If there is one, it is always at len-7)
		// if ftos had fucked it up, which should never happen: "34278.000000"
	if(len >= 7)
		if(substring(result, len - 7, 1) == ".")
		{
			dprint("ftos(integer) has comma? Can't be. Affected result: ", result, "\n");
			result = substring(result, 0, len - 7);
			len -= 7;
		}
		// "34278"
	if(decimals == 0)
		return result; // don't insert a point for zero decimals
	// is it too short? If yes, insert leading zeroes
	if(len <= decimals)
	{
		result = strcat(substring("0000000000", 0, decimals - len + 1), result);
		len = decimals + 1;
	}
	// and now... INSERT THE POINT!
	tmp = substring(result, len - decimals, decimals);
	result = strcat(substring(result, 0, len - decimals), ".", tmp);
	return result;
}

float time;
vector colormapPaletteColor(float c, float isPants)
{
	switch(c)
	{
		case  0: return '0.733 0.733 0.733';
		case  1: return '0.451 0.341 0.122';
		case  2: return '0.000 0.733 0.733';
		case  3: return '0.000 1.000 0.000';
		case  4: return '1.000 0.000 0.000';
		case  5: return '0.000 0.502 1.000';
		case  6: return '0.812 0.561 0.169';
		case  7: return '0.718 0.529 0.420';
		case  8: return '0.765 0.545 0.667';
		case  9: return '1.000 0.000 1.000';
		case 10: return '0.639 0.529 0.482';
		case 11: return '0.310 0.388 0.341';
		case 12: return '1.000 1.000 0.000';
		case 13: return '0.000 0.000 1.000';
		case 14: return '1.000 0.502 0.000';
		case 15:
			if(isPants)
				return
					  '1 0 0' * (0.502 + 0.498 * sin(time / 2.7182818285 + 0.0000000000))
					+ '0 1 0' * (0.502 + 0.498 * sin(time / 2.7182818285 + 2.0943951024))
					+ '0 0 1' * (0.502 + 0.498 * sin(time / 2.7182818285 + 4.1887902048));
			else
				return
					  '1 0 0' * (0.502 + 0.498 * sin(time / 3.1415926536 + 5.2359877560))
					+ '0 1 0' * (0.502 + 0.498 * sin(time / 3.1415926536 + 3.1415926536))
					+ '0 0 1' * (0.502 + 0.498 * sin(time / 3.1415926536 + 1.0471975512));
		default: return '0.000 0.000 0.000';
	}
}

// unzone the string, and return it as tempstring. Safe to be called on string_null
string fstrunzone(string s)
{
	string sc;
	if not(s)
		return s;
	sc = strcat(s, "");
	strunzone(s);
	return sc;
}

// Databases (hash tables)
#define DB_BUCKETS 8192
void db_save(float db, string pFilename)
{
	float fh, i, n;
	fh = fopen(pFilename, FILE_WRITE);
	if(fh < 0)
		error(strcat("Can't write DB to ", pFilename));
	n = buf_getsize(db);
	fputs(fh, strcat(ftos(DB_BUCKETS), "\n"));
	for(i = 0; i < n; ++i)
		fputs(fh, strcat(bufstr_get(db, i), "\n"));
	fclose(fh);
}

float db_create()
{
	return buf_create();
}

float db_load(string pFilename)
{
	float db, fh, i, j, n;
	string l;
	db = buf_create();
	if(db < 0)
		return -1;
	fh = fopen(pFilename, FILE_READ);
	if(fh < 0)
		return db;
	if(stof(fgets(fh)) == DB_BUCKETS)
	{
		i = 0;
		while((l = fgets(fh)))
		{
			if(l != "")
				bufstr_set(db, i, l);
			++i;
		}
	}
	else
	{
		// different count of buckets?
		// need to reorganize the database then (SLOW)
		while((l = fgets(fh)))
		{
			n = tokenizebyseparator(l, "\\");
			for(j = 2; j < n; j += 2)
				db_put(db, argv(j-1), argv(j));
		}
	}
	fclose(fh);
	return db;
}

void db_dump(float db, string pFilename)
{
	float fh, i, j, n, m;
	fh = fopen(pFilename, FILE_WRITE);
	if(fh < 0)
		error(strcat("Can't dump DB to ", pFilename));
	n = buf_getsize(db);
	fputs(fh, "0\n");
	for(i = 0; i < n; ++i)
	{
		m = tokenizebyseparator(bufstr_get(db, i), "\\");
		for(j = 2; j < m; j += 2)
			fputs(fh, strcat("\\", argv(j-1), "\\", argv(j), "\n"));
	}
	fclose(fh);
}

void db_close(float db)
{
	buf_del(db);
}

string db_get(float db, string pKey)
{
	string lRawKey;
	lRawKey = strreplace( chr2str( 128 + 92 ), "\\", pKey );
	float h;
	h = mod(crc16(FALSE, pKey), DB_BUCKETS);
	string lRawValue;
	lRawValue = infoget(bufstr_get(db, h), pKey);
	return strreplace( "\\", chr2str( 128 + 92 ), lRawValue );
}

void db_put(float db, string pKey, string pValue)
{
	string lRawKey;
	lRawKey = strreplace( chr2str( 128 + 92 ), "\\", pKey);
	string lRawValue;
	lRawValue = strreplace( chr2str( 128 + 92 ), "\\", pValue );

	float h;
	h = mod(crc16(FALSE, pKey), DB_BUCKETS);
	bufstr_set(db, h, infoadd(bufstr_get(db, h), lRawKey, lRawValue));
}

void db_test()
{
	float db, i;
	print("LOAD...\n");
	db = db_load("foo.db");
	print("LOADED. FILL...\n");
	for(i = 0; i < DB_BUCKETS; ++i)
		db_put(db, ftos(random()), "X");
	print("FILLED. SAVE...\n");
	db_save(db, "foo.db");
	print("SAVED. CLOSE...\n");
	db_close(db);
	print("CLOSED.\n");
}

// Multiline text file buffers
float buf_load(string pFilename)
{
	float buf, fh, i;
	string l;
	buf = buf_create();
	if(buf < 0)
		return -1;
	fh = fopen(pFilename, FILE_READ);
	if(fh < 0)
		return buf;
	i = 0;
	while((l = fgets(fh)))
	{
		bufstr_set(buf, i, l);
		++i;
	}
	fclose(fh);
	return buf;
}

void buf_save(float buf, string pFilename)
{
	float fh, i, n;
	fh = fopen(pFilename, FILE_WRITE);
	if(fh < 0)
		error(strcat("Can't write buf to ", pFilename));
	n = buf_getsize(buf);
	for(i = 0; i < n; ++i)
		fputs(fh, strcat(bufstr_get(buf, i), "\n"));
	fclose(fh);
}

// dont str-zone the string
string str_load( string pFilename ) {
	float fh, i;
	string l;
	string lText;
	fh = fopen(pFilename, FILE_READ);
	if(fh < 0)
		return "";
	i = 0;
	lText = "";
	while((l = fgets(fh)))
	{
		lText = strcat( lText, l, "\n" );
		++i;
	}
	fclose(fh);
	return lText;
}

void str_save(string lContent, string pFilename)
{
	float fh;
	fh = fopen(pFilename, FILE_WRITE);
	if(fh < 0)
		error(strcat("Can't write buf to ", pFilename));
	fputs(fh, lContent);
	fclose(fh);
}
