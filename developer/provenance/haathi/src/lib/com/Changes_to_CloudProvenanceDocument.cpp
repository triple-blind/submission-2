void CloudProvenanceDocument::addFileHashTableEvents()
std::vector<std::string> fileModules = GetFileHashTable();
     
    std::vector<std::string>::iterator it2;

    for (it2 = fileModules.begin(); it2 != fileModules.end(); ++it2)
        addProvenanceEvent(new FileHashDocumentProvenance(*it));