#ifndef DATAHANDLER_HH
#define DATAHANDLER_HH

namespace ub_pion_trajectory 
{
   class DataHandler 
   {
   public:
      static DataHandler* Instance();
      ~DataHandler();

      DataHandler(const DataHandler&) = delete;
      DataHandler& operator=(const DataHandler&) = delete;

      void AddEntry();
      void WriteFile();
      void Reset();

   private:
      DataHandler(const char* filename = "Data/output.root");

      static DataHandler* m_Instance;

      TFile* m_RootFile;
      TTree* m_EventTree;
   };
}

#endif  // DATAHANDLER_HH