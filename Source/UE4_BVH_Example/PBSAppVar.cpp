#include "PBSAppVar.h"
#include "PBS/MotionDBforPBS.h"
#include "EngineUtils.h"			// Debug 출력을 위해...

PBSAppVar* PBSAppVar::singleton_ = 0;

//#define PBS_DATA_PATH "C:/Mingle/Research/Taesung/UE4_BVH_Example-master/data/"
#define PBS_DATA_PATH "C:/Users/cucg/Desktop/VrPosing-master/data/"

PBSAppVar::PBSAppVar()
{
	this->selected_body_joint = PBS::SketchedQuery::J_NONE;
	flag_plus_IK_ = false;
	flag_IK_only_ = false;
	flag_hide_char_ = false;

	

	motion_db_ = new PBS::MotionDBforPBS;

	//if (!PBSAppVar::getSingleton()->motion_db->loadBinaryFile("data/motion_db.db"))
	//if ( motion_db_->size() == 0)
	{
		//if (!PBSAppVar::getSingleton()->motion_db->loadBinaryFile(PBS_DATA_PATH"motion_db.db"))
		{
			motion_db_->addMotionDir(PBS_DATA_PATH"motion_100_num/basketball_12/mghys.asf", PBS_DATA_PATH"motion_100_num/basketball_12/", "mghys");
			motion_db_->addMotionDir(PBS_DATA_PATH"motion_100_num/bboy_4/BBoy_re.asf", PBS_DATA_PATH"motion_100_num/bboy_4/", "BBoy_re");
			motion_db_->addMotionDir(PBS_DATA_PATH"motion_100_num/emotional_7/mgman.asf", PBS_DATA_PATH"motion_100_num/emotional_7/", "mgman");
			motion_db_->addMotionDir(PBS_DATA_PATH"motion_100_num/environment_3/wd2.asf", PBS_DATA_PATH"motion_100_num/environment_3/", "wd2");
			motion_db_->addMotionDir(PBS_DATA_PATH"motion_100_num/freestyle_dance_12/wd2.asf", PBS_DATA_PATH"motion_100_num/freestyle_dance_12/", "wd2");
			motion_db_->addMotionDir(PBS_DATA_PATH"motion_100_num/generals_17/mgmg.asf", PBS_DATA_PATH"motion_100_num/generals_17/", "mgmg");
			motion_db_->addMotionDir(PBS_DATA_PATH"motion_100_num/glof_2/wd2.asf", PBS_DATA_PATH"motion_100_num/glof_2/", "wd2");
			motion_db_->addMotionDir(PBS_DATA_PATH"motion_100_num/locomotions_12/m1.asf", PBS_DATA_PATH"motion_100_num/locomotions_12/", "m1");
			motion_db_->addMotionDir(PBS_DATA_PATH"motion_100_num/table_tennis_6/wd2.asf", PBS_DATA_PATH"motion_100_num/table_tennis_6/", "wd2");
			motion_db_->addMotionDir(PBS_DATA_PATH"motion_100_num/talking_with_12/wd2.asf", PBS_DATA_PATH"motion_100_num/talking_with_12/", "wd2");
			motion_db_->addMotionDir(PBS_DATA_PATH"motion_100_num/tawkwon_15/wd2.asf", PBS_DATA_PATH"motion_100_num/tawkwon_15/", "wd2");
			//motion_db_->addMotionDir(PBS_DATA_PATH"etc/wd2.asf", PBS_DATA_PATH"etc", "wd2", 0);
			//motion_db_->addMotionDir(PBS_DATA_PATH"etc/itonic3_wd2.asf", PBS_DATA_PATH"etc", "itonic3_wd2", 0);


			for ( auto &d : *motion_db_ )
			{
				d->editable_body()->SetPredefinedTagMap("woody");
			}

			motion_db_->SampleFrames();

			//PBSAppVar::getSingleton()->motion_db->saveBinaryFile("data/motion_db.db");
			//UE_LOG(LogTemp, Warning, TEXT("bad"));
		}
		/*	else
		{
		UE_LOG(LogTemp, Warning, TEXT("good"));

		}*/
	}
}

PBSAppVar::~PBSAppVar()
{
	if ( motion_db_ !=0 ) delete motion_db_;
	motion_db_ = 0;
}

PBSAppVar*
PBSAppVar::getSingleton()
{
	if ( singleton_ == 0 ) 
		singleton_ = new PBSAppVar;

	return singleton_;
}

void PBSAppVar::DestroySingleton()
{
	if ( singleton_ != 0 ) delete singleton_;
	singleton_ = 0;
}

