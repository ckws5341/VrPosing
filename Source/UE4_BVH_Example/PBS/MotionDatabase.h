
#pragma once

//#include "PmQm/pm.h"
//#include <string>
//#include <map>
//#include <fstream>
//
//namespace PBS
//{
//
//class MotionNameMap : public std::map<std::string, PmLinearMotion*>
//{
//};
//
//
//class MotionDatabase  
//{
//public:
//	MotionDatabase();
//
//	virtual void addMotionDir(std::string actor_file, std::string dir_path, std::string prefix_of_file, unsigned int max_byte=0, int max_file_num=0);
//	virtual void addMotionFile(std::string actor_file, std::string motion_file, std::string name="");
//	virtual void addMotion(PmLinearMotion *motion, std::string name="");
//
//	bool isExist(PmLinearMotion *m);
//	bool isExist(std::string name);
//
//	std::string getName(int i);
//	const PmLinearMotion* getMotion(int i);
//	const PmLinearMotion *getMotion(std::string name);
//	PmLinearMotion* getEditableMotion(int i);
//	PmLinearMotion *getEditableMotion(std::string name);
//
//	int getSize() { return getNumMotionFiles(); }
//	int getNumMotionFiles() { return motion_map.size(); }
//	int getNumTotalFrames();
//
//	virtual void saveBinaryFile(std::string filename);
//	virtual bool loadBinaryFile(std::string filename);
//
//	virtual void writeBinary(std::ostream &out);
//	virtual void readBinary(std::istream &in);
//
//protected:
//	std:: string getFilenameFromPath(std::string path);
//
//
//protected:
//	
//	MotionNameMap motion_map;
//};
//
//
//};
