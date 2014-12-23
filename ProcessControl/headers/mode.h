#ifndef MODE_H_INCLUDED__
#define MODE_H_INCLUDED__

#include <string>
#include <vector>
#include <sstream>
#include <istream>

using std::string;
using std::vector;
using std::stringstream;

class ProcessDesc {
private:
    string path;
    string name;
public:
    ProcessDesc(string path);
    string getPath() const;
    string getName() const;
};

/**
* @struct  Mode
*
* @brief   Strucure, which describes mode - set of "rules" - programs and
*          urls, which cannot be opened during mode run, and progress (0 - 100%) -
*          if user want to track progress of some task.
*/
class Mode {
    static const char DELIMITER = '|';
    string name;
    string description;
    /** @brief   Processes, which will be controlled */
    vector<ProcessDesc> processes;
    /** @brief   Urls, which will be restricted to open */
    vector<string> urls;
    short progress;

public:
    /** @brief   Number of lines in printed mode with operator<< */
    static int const STR_MODE_LINE_COUNT = 5;
    Mode();

    /**
     * @fn  Mode::Mode(stringstream stream);
     *
     * @brief   Constructs mode object from string stream
     *
     * @param   stream  stream with mode packed into string with
     *                  operator<<
     */
    Mode(stringstream &stream);

    string getName() const;
    string getDescription() const;
    const vector<string>& getUrls() const;
    const vector<ProcessDesc>& getProcesses() const;
    short getProgress() const;
    size_t getUrlCount() const;
    size_t getProcessCount() const;

    void setName(string name);
    void setDescription(string description);
    void setProgress(short progress);
    void addUrl(string url);
    void addProcess(ProcessDesc pd);
    void addProcess(string path);

    friend std::ostream& operator<< (std::ostream& stream, const Mode& mode);
};

#endif