//
//#include "MotionDatabase.h"
//#include <algorithm>
//#include <windows.h>
//
//namespace PBS
//{
//
//MotionDatabase::MotionDatabase()
//{
//}
//
//
//
//
//
//
//std::string 
//MotionDatabase::getName(int i)
//{
//	int ii=0;
//
//	MotionNameMap::iterator iter;
//	for ( iter=motion_map.begin(); iter!=motion_map.end(); iter++ )
//	{
//		if ( ii == i ) return (*iter).first;
//		ii++;
//	}
//
//	return 0;
//}
//
//const PmLinearMotion* 
//MotionDatabase::getMotion(int i)
//{
//	return getEditableMotion(i);
//}
//
//
//const PmLinearMotion*
//MotionDatabase::getMotion(std::string name)
//{
//	return getEditableMotion(name);
//}
//
//
//PmLinearMotion* 
//MotionDatabase::getEditableMotion(int i)
//{
//	int ii=0;
//
//	MotionNameMap::iterator iter;
//	for ( iter=motion_map.begin(); iter!=motion_map.end(); iter++ )
//	{
//		if ( ii == i ) return (*iter).second;
//		ii++;
//	}
//
//	return 0;
//}
//
//PmLinearMotion*
//MotionDatabase::getEditableMotion(std::string name)
//{
//	if ( motion_map.find(name) == motion_map.end() ) return 0;
//	return motion_map[name];
//}
//
//
//bool
//MotionDatabase::isExist(std::string name)
//{
//	if ( motion_map.find(name) == motion_map.end() ) return false;
//	return true;
//}
//
//
//bool
//MotionDatabase::isExist(PmLinearMotion *m)
//{
//	MotionNameMap::iterator i;
//	for ( i=motion_map.begin(); i!=motion_map.end(); i++ )
//	{
//		if ( i->second == m ) return true;
//	}
//
//	return false;
//}
//
//int
//MotionDatabase::getNumTotalFrames()
//{
//	int t=0;
//	MotionNameMap::iterator i;
//	for ( i=motion_map.begin(); i!=motion_map.end(); i++ )
//	{
//		t += i->second->getSize();
//	}
//
//	return t;
//}
//
//
//
//void 
//MotionDatabase::addMotion(PmLinearMotion *motion, std::string name)
//{
//	if ( isExist(motion) ) return;
//
//	// generate an exclusive motion name.
//	if ( name.size() == 0 || isExist(name) )
//	{
//		char tmp_str[256];
//		{
//			int tmp_number = motion_map.size();
//			_itoa_s(tmp_number, tmp_str, 10);
//		}
//
//		name.append(tmp_str);
//
//		while ( isExist(name) )
//		{
//			name.append("_");
//		}
//
//	}
//
//	motion_map[name] = motion;
//}
//
//
//
//std::string
//MotionDatabase::getFilenameFromPath(std::string path)
//{
//	std::string::size_type end_i = path.rfind(".")-1;
//	std::string::size_type start_i =  path.rfind("/")+1;
//	return path.substr(start_i, end_i-start_i+1);
//}
//
//
//
//void 
//MotionDatabase::addMotionFile(std::string actor_file, std::string motion_file, std::string name)
//{
//	PmHuman *human = new PmHuman(actor_file.c_str());
//	PmLinearMotion *motion = new PmLinearMotion(human);
//	motion->openAMC_Woody(motion_file.c_str());
//
//	if ( actor_file.find("mgmg.actor") != std::string::npos ) 
//	{
//		for ( int i=0; i<(int)motion->getSize(); i++ )
//		{
//			motion->getPosture(i).addRotation(PmHuman::NECK, ::vector(M_PI/6, 0, 0));
//		}
//	}
//
//	if ( actor_file.find("mghys.actor") != std::string::npos ) 
//	{
//		for ( int i=0; i<(int)motion->getSize(); i++ )
//		{
//			motion->getPosture(i).addRotation(PmHuman::NECK, ::vector(-1*M_PI/6, 0, 0));
//		}
//	}
//
//	if ( (int)name.size()==0 )
//		name = getFilenameFromPath(motion_file);
//
//	addMotion(motion, name);
//
//	addMotion(motion, name);
//
//	printf("%s, %s\n", name.c_str(), motion_file.c_str());
//
//}
//
//
//void 
//MotionDatabase::addMotionDir(std::string actor_file, std::string dir_path, std::string prefix_of_file, unsigned int max_byte, int max_file_num)
//{
//	PmHuman *human = new PmHuman(actor_file.c_str());
//
//	WIN32_FIND_DATAA find_file_data;
//	HANDLE h_find;
//
//	if ( *(dir_path.end()-1) != '/' )
//		dir_path.append("/");
//
//	std::string reg_exp = dir_path;
//	if ( (int)prefix_of_file.size() > 0 )
//		reg_exp.append(prefix_of_file);
//
//	reg_exp.append("*.amc");
//
//	int count = 0;
//
//	h_find = FindFirstFileA(reg_exp.c_str(), &find_file_data);		
//	if ( h_find != INVALID_HANDLE_VALUE )
//	{
//		do
//		{
//			if ( max_byte <= 0
//				|| 
//				( find_file_data.nFileSizeHigh == 0
//				&&
//				max_byte > find_file_data.nFileSizeLow )
//				)
//			{
//				std::string file = dir_path + find_file_data.cFileName;
//				addMotionFile(actor_file, file);
//
//				count++;
//
//				if ( max_file_num > 0 && max_file_num <= count ) break;
//			}
//		}
//		while ( FindNextFileA(h_find, &find_file_data) );
//
//	}
//	FindClose(h_find);
//}
//
//
//void
//MotionDatabase::saveBinaryFile(std::string filename)
//{
//	std::ofstream fout(filename, std::ios::binary);
//	writeBinary(fout);
//	fout.close();
//}
//
//bool
//MotionDatabase::loadBinaryFile(std::string filename)
//{
//	std::ifstream fin(filename, std::ios::binary);
//	if ( fin.fail() )
//	{
//		fin.close();
//		return false;
//	}
//	readBinary(fin);
//	fin.close();
//	return true;
//}
//
//void
//MotionDatabase::writeBinary(std::ostream &out)
//{
//	int data_size = (int)motion_map.size();
//	out.write((char*)&data_size, sizeof(data_size));
//
//	MotionNameMap::iterator iter;
//	for ( iter=this->motion_map.begin(); iter!=motion_map.end(); iter++ )
//	{
//		int name_len = iter->first.size();
//		out.write((char*)&name_len, sizeof(name_len));
//		out.write((char*)iter->first.c_str(), iter->first.size());
//		iter->second->saveBinary(out);
//	}
//}
//
//void
//MotionDatabase::readBinary(std::istream &in)
//{
//	motion_map.clear();
//
//	int data_size;
//	in.read((char*)&data_size, sizeof(data_size));
//
//	for ( int i=0; i<data_size; i++ )
//	{
//		int name_len;
//		in.read((char*)&name_len, sizeof(name_len));
//		char name[256];
//		in.read((char*)name, name_len);
//		name[name_len] = '\0';
//
//		printf("%d, %s\n", name_len, name);
//
//		PmLinearMotion *motion = new PmLinearMotion;
//		motion->loadBinary(in);
//
//		//std::string name_str = name;
//		motion_map[name] = motion;
//	}
//}
//
//
//
//};