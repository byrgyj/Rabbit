#pragma once
/*
author: byrgyj@126.com
date: 2018-4-15
*/
class BaseInterface{
public:

	virtual bool seekTo(int millsec) = 0;
	virtual int getData(char *buffer, int bufSize) = 0;
	virtual int writeData(char *data, int sz) = 0;
};

