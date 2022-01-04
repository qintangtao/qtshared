#ifndef BASICCONFIGURATOR_H
#define BASICCONFIGURATOR_H

class Appender;

class BasicConfigurator
{
private:
    BasicConfigurator(); // Not implemented
    // BasicConfigurator(const BasicConfigurator &rOther); // Use compiler default
    // virtual ~BasicConfigurator(); // Use compiler default
    // BasicConfigurator &operator=(const BasicConfigurator &rOther); // Use compiler default

public:
    static bool configure();
    static void configure(Appender *pAppender);
    static void resetConfiguration();
};

#endif // BASICCONFIGURATOR_H
