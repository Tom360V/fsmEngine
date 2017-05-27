import os
import sys
import shutil
import re
import collections
import fileinput
from datetime import datetime, date, time
from os.path import basename

#
#   Statemachine -> States  -> TransitionState (Inherit from TransitionChoice)
#                           -> TransitionChoice 
#

scriptDir="./"
listStateTypes = ['STATE', 'CHOICE'];
template_directory = "templates/"
transitionTuple = collections.namedtuple("CmdItem", ["keyword", "follow", "rest"])


GuardBodyTemplate  = """
/*******************************************************************************
 * GUARD
 */
int8_t [guard](void *p)
{
    (void)p;
    //enter your code here
    return FSM_FALSE;
}\n"""


ActionBodyTemplate  = """
/*******************************************************************************
 * ACTION
 */
void [action](void *p)
{
    (void)p;
    //enter your code here
}\n"""
#------------------------------------------------------------------------------
class TransitionChoice:
    def __init__(self, guards, actions, nextState):
        self.guards = []
        self.type = "CHOICE"

        guards = guards.replace('\t', '')
        guards = guards.replace(' ', '')
        self.guards = guards.split(',')
        if(self.guards[0]==''):
            self.guards = []

        actions = actions.replace('\t', '')
        actions = actions.replace(' ', '')
        self.actions = actions.split(',')
        if(self.actions[0]==''):
            self.actions = []
            
        self.nextState = "eState" + nextState.strip()

    def dump(self):
        print("  {0}".format(self.type))
        print("    guards:    ")
        for g in self.guards:
            print("               {0}".format(g))
        print("    actions:   ")
        for a in self.actions:
            print("               {0}".format(a))
        print("    nextState: {0}".format(self.nextState))
       
    def getType(self):
        return self.type

    def getActionList(self):
        actionsList = []
        for a in self.actions:
            if(a != ''):
                actionsList.append(a)
        return actionsList

    def getGuardList(self):
        guards = []
        for g in self.guards:
            if( (g != '') and
                (g != 'ELSE') ):
                guards.append(g)
        return guards

    def GetTransitionRow(self, name):
        template = "\n\t{\t[NEXTSTATE],\n\t\t{[GUARDS_NOF], (FSM_fpGuard_t []){[GUARDS]} },\n\t\t{[ACTIONS_NOF], (FSM_fpAction_t[]){[ACTIONS]} }\n\t},"
        transition = template.replace  ('[NEXTSTATE]'   ,name + "_" + self.nextState)

        s = name+"_"
        actionStr = s+(', '+s).join(self.actions)
        if(actionStr==s): actionStr = 'NULL'
        transition = transition.replace('[ACTIONS]'     , actionStr)
        transition = transition.replace('[ACTIONS_NOF]' , str(len(self.actions)))

        guardStr = s+(', '+s).join(self.guards)
        if(guardStr==s): guardStr = 'NULL'
        transition = transition.replace('[GUARDS]'      , guardStr)
        
        transition = transition.replace('[GUARDS_NOF]'  , str(len(self.guards)))
        return transition

#------------------------------------------------------------------------------
class TransitionState(TransitionChoice):
    def __init__(self, event, guards, actions, nextState):
        TransitionChoice.__init__(self, guards, actions, nextState)
        self.type = "State"
        self.event = "e"+event.strip()

    def getEvent(self):
        return self.event
       
    def dump(self):
        TransitionChoice.dump(self)
        print("    Event:     {0}".format(self.event))

    def GetTransitionRow(self, name):
        transition = TransitionChoice.GetTransitionRow(self, name)
        return transition.replace('{'  ,"{\t" + name + "_" + self.event + ", ",1)

#------------------------------------------------------------------------------
class State:
    def __init__(self, parent, name, type):
        self.params = []
        splitname = name.split('::')
        self.parentName = parent.name
        self.name = parent.name + "_State" + splitname[0]
        if (len(splitname)>1):
            self.params = splitname[1]
        
        self.type = type
        self.transitions = []

    def addTransition(self, row):
        row.rstrip()
        if (row.startswith('{') and row.endswith('}') ):
            row = row.replace('{','')
            row = row.replace('}','')
            part = row.split(';')

            if self.type == 'STATE':
                t = TransitionState(part[0], part[1], part[2], part[3])
            elif self.type == 'CHOICE':
                t = TransitionChoice(part[0], part[1], part[2])

            self.transitions.append(t)
#        else:
#            print "addTransition: ERROR,"
    
    def dump(self):
        print("type: {0}".format(self.type))
        print("name: {0}".format(self.name))
        print("transitions: {0}".format(str(len(self.transitions))))
        for t in self.transitions:
            t.dump()

    def getActions(self):
        actions = []
        for a in self.transitions:
            actions += a.getActionList()
        return list(set(actions))       #Remove duplicates

    def getEvents(self):
        events = []
        for t in self.transitions:
            if (t.getType() != 'CHOICE'):
                events.append(t.getEvent())
        return list(set(events))        #Remove duplicates

    def getGuards(self):
        guards = []
        for t in self.transitions:
            guards += t.getGuardList()
        return list(set(guards))        #Remove duplicates

    def getNameAsEnum(self):
        return self.name.replace('_', '_e')

    def getTransitionRows(self):
        tString = ""
        for T in self.transitions:
            tString += T.GetTransitionRow(self.parentName)
        return tString

#------------------------------------------------------------------------------
class Statemachine:
    def __init__(self, name, outputdir):
        self.States = []
        self.name = os.path.basename(name.replace('.sm',''))
        print("statemachine name: {0}".format(self.name))
        print("output directory: {0}" .format(outputdir))

        self.outputdir = outputdir
        if(not self.outputdir.endswith('/')):
            self.outputdir += '/'
        if(not os.path.isdir(self.outputdir)):
            os.mkdir(self.outputdir)
        if(not os.path.isdir(self.outputdir+"/inc")):
            os.mkdir(self.outputdir+"/inc")
        if(not os.path.isdir(self.outputdir+"/src")):
            os.mkdir(self.outputdir+"/src")
    
    def addState(self, _state):
        self.States.append(_state)

    def getActions(self):
        ActionList = []
        for S in self.States:
            ActionList += S.getActions()
        ActionList = sorted(list(set(ActionList)))  #Remove duplicates
        return [self.name +"_" + x for x in ActionList]

    def getEvents(self):
        EventList = []
        for S in self.States:
            EventList += S.getEvents()
        EventList = list(set(EventList))            #Remove duplicates
        return [self.name +"_" + x for x in EventList]

    def getGuards(self):
        GuardList = []
        for S in self.States:
            GuardList += S.getGuards()
        GuardList = sorted(list(set(GuardList)))    #Remove duplicates
        return [self.name +"_" + x for x in GuardList]

    def getStateNames(self):
        StateList = []
        for S in self.States:
            StateList.append(S.name)
        return StateList

    def getStateNamesAsEnum(self):
        StateList = []
        for S in self.States:
            StateList.append(S.getNameAsEnum())
        return StateList

    def getStartState(self):
        for S in self.States:
            if( (len(S.params) > 0) and
                (S.params.find("START") >= 0) ):
                return S.getNameAsEnum()
        return "ERROR"
#------------------------------------------------------------------------------
def CopyTemplate(template, fname, overwrite):
    if(os.path.isfile(fname)):
        if (overwrite == "NEVER_OVERWRITE"):
            print("\nCopy template [create new]")
            fname += "_NEW"
        else:
            print("\nCopy template [overwrite]")
    else:
        print("\nCopy template")


    print("    from: {0}".format(scriptDir+template_directory+template))
    print("    to: {0}"  .format(fname))

    shutil.copyfile(scriptDir+template_directory+template, fname)

    dt = datetime.now()
    
    ReplaceInFile(fname, "[DATE]", dt.strftime("%A %d. %B %Y"))
    ReplaceInFile(fname, "[TIME]", dt.strftime("%H:%M:%S"))
    return fname

#------------------------------------------------------------------------------
def ReplaceInFile(fname, old, new):
#    print "ReplaceInFile:: " + fname + " " + old + "\n" + new
    for i, line in enumerate(fileinput.input(fname, inplace=1)):
        sys.stdout.write(line.replace(old, new))

#------------------------------------------------------------------------------
def CreatePrivateHeaderFile(_SM):
    f = _SM.outputdir+"src/"+_SM.name+"_sm_private.h"
    f = CopyTemplate("sm_private.h", f, "OVERWRITE")
    print("\nCreate private header file: {0}".format(f))
    
    ReplaceInFile(f, "[filename]", _SM.name)
    ReplaceInFile(f, "[filename_capital]",_SM.name.upper() )

    # Get all Events and convert to string which can be inserted into the template
    EventList = _SM.getEvents()
    EventList = ["    " + x for x in EventList]
    ReplaceInFile(f, "[EVENTS]", (",\n".join(EventList)).replace(",", " = 0,", 1))

    #Get al Guards and create function prototypes for them
    GuardList = _SM.getGuards()
    GuardList = ["int8_t " + x + "(void *p);" for x in GuardList]
    ReplaceInFile(f, "[GUARDS]", "\n".join(GuardList))
    
    #Get al Actions and create function prototypes for them
    ActionList = _SM.getActions()
    ActionList = ["void " + x + "(void *p);" for x in ActionList]
    ReplaceInFile(f, "[ACTIONS]", "\n".join(ActionList))

    #Generate Statetables
    stateTables = ""
    for State in _SM.States:
        stateTables += "static const FSMTransition_t " + State.name + "[] = {"
        stateTables +=  State.getTransitionRows()
        stateTables += "\n};\n\n"
    ReplaceInFile(f, "[STATETABLES]", stateTables)

    # Get all States as Enum and convert then to a string which can be inserted into the template
    states = ["    " + x + "," for x in _SM.getStateNamesAsEnum()]
    ReplaceInFile(f, "[STATES]",  (("\n".join(states)).replace(",", " = 0,", 1)))
    
    # Get all States as Enum and add them to stateTable
    StatesList = ["FSM_ADDSTATE(" + x + ")" for x in _SM.getStateNames()]
    ReplaceInFile(f, "[ADD_STATES]",  (", ".join(StatesList)))
    ReplaceInFile(f, "[STARTSTATE]", _SM.getStartState())
    return 0

#------------------------------------------------------------------------------
def CreateBodyFile(_SM):
    f = _SM.outputdir+"src/"+_SM.name+"_sm.c"
   
    f = CopyTemplate("sm.c", f, "NEVER_OVERWRITE")
    print("\nCreate body file: {0}".format(f))
    
    ReplaceInFile(f, "[filename]", _SM.name)

    #Get al Guards and create functions for them
    GuardList = _SM.getGuards()
    GuardsString = ""
    for G in GuardList:
        temp = GuardBodyTemplate.replace("[guard]",G)
        GuardsString += temp
    ReplaceInFile(f, "[GUARDS]", GuardsString)

    #Get al Actions and create functions for them
    ActionList = _SM.getActions()
    ActionsString = ""
    for A in ActionList:
        temp = ActionBodyTemplate.replace("[action]",A)
        ActionsString += temp
    ReplaceInFile(f, "[ACTIONS]", ActionsString)

    EventList = _SM.getEvents()
    EventList = ["     *   - " + x for x in EventList]
    ReplaceInFile(f, "[EVENTS]", ",\n".join(EventList))


#------------------------------------------------------------------------------
def CreateHeaderFile(_SM):
    f = _SM.outputdir+"inc/"+_SM.name+"_sm.h"
    f = CopyTemplate("sm.h", f, "NEVER_OVERWRITE")
    print("\nCreate header file: {0}".format(f))
    ReplaceInFile(f, "[filename]", _SM.name)
    ReplaceInFile(f, "[filename_capital]",_SM.name.upper() )
    
#------------------------------------------------------------------------------
def RowContainsStartOfState(_row):
    keyword = _row.split(' ')
    if (keyword[0] in listStateTypes):
        return keyword[0], keyword[1]
    return 0
    
#------------------------------------------------------------------------------
def RowContainsEndOfState(row):
    if(row[0] == '}'):
        return 1
    return 0
    
#------------------------------------------------------------------------------
def ReadFile(fname, _SM):
    with open(fname,'r') as f:
        filecontent = f.readlines()
    
    idx = 1;
    S = 0
    type = 'unknown'
    name = 'unknown'
    
    for row in filecontent:
        row = row.rstrip('\n')
        row = row.strip(' ')
        row = row.strip('\t')
        if( (len(row)>0) and
            (row.startswith('//') == False ) ):
        
            if(idx == 1):         #Search start of State
                type, name = RowContainsStartOfState(row)
                if (type != 0) :
                    S = State(_SM, name, type)
                    idx = 2

            elif (idx == 2):      #Get all transitions
                if RowContainsEndOfState(row):
                    _SM.addState(S)
                    idx = 1
                    type = 'unknown!'
                    name = 'unknown!'
                else:               #add transition
                    S.addTransition(row)
                    
#==============================================================================
if __name__ == "__main__":
    filename=""
    outputdir=""
    extention = ".sm"

    if len(sys.argv) > 2:
        filename = sys.argv[1]
        outputdir = sys.argv[2]
    else:
        print("usage: {0} [statemachine file] [outputdir]".format(sys.argv[0]))
        exit(1)

    if not filename.endswith(extention):
        print("Invalid extention, expecting '{0}'".format(extention))
        exit(1)

    if (os.path.isfile(filename)==False):
        print("Invalid file: {0}".format(filename))
        exit(1)

    scriptDir = os.path.dirname(sys.argv[0]) + "/";
    print("scriptDir: {0}".format(scriptDir))
    
    ST = Statemachine(filename, outputdir)

    ReadFile(filename, ST)

    CreatePrivateHeaderFile(ST)
    CreateBodyFile(ST)
    CreateHeaderFile(ST)
