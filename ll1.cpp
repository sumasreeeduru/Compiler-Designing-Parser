#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <unordered_map>
#define VARIABLE 1
#define ARROW 2
#define PRODUCTIONS 3
#define PIPE 4
#define NEW_LINE 5

using namespace std;
extern int yylex();
extern int yylineno;
extern char *yytext;

unordered_map<char, int> nonmpt;
unordered_map<char, int> mpt;

bool isTerminal(char c)
{

    if (nonmpt.find(c) == nonmpt.end())
        return true;
    return false;
}

void findmpt(map<char, vector<string>> context_free_grammar)
{

    for (auto it1 = context_free_grammar.begin(); it1 != context_free_grammar.end(); it1++)
    {

        for (auto it2 = it1->second.begin(); it2 != it1->second.end(); it2++)
        {

            string prod = *it2;
            for (int i = 0; i < prod.size(); i++)
            {

                if (isTerminal(prod[i]))
                    mpt[prod[i]] = 1;
            }
        }
    }
}

void pretty_print(map<char, vector<char>> context_free_grammar)
{

    map<char, vector<char>>::iterator it1;
    vector<char>::iterator it2;

    for (it1 = context_free_grammar.begin(); it1 != context_free_grammar.end(); it1++)
    {

        cout << it1->first << "={";
        int i = 0;
        for (it2 = it1->second.begin(); it2 != it1->second.end(); it2++)
        {

            if (i++ != 0)
                cout << ",";

            cout << *it2;
        }

        cout << "}" << endl;
    }
}

void printMap(map<char, vector<string>> context_free_grammar)
{

    for (auto it1 = context_free_grammar.begin(); it1 != context_free_grammar.end(); it1++)
    {

        cout << it1->first << "=";
        int start = 0;

        unordered_map<string, int> tmp;

        for (auto it2 = it1->second.begin(); it2 != it1->second.end(); it2++)
        {

            if (tmp.find(*it2) == tmp.end())
            {

                if (start++ != 0)
                    cout << "/";
                cout << *it2;
            }

            tmp[*it2] = 1;
        }
        cout << endl;
    }
}

vector<char> Combine(vector<char> s1, vector<char> s2)
{

    unordered_map<char, int> tmp;

    vector<char> res;

    for (auto it = s1.begin(); it != s1.end(); it++)
    {
        if (tmp.find(*it) == tmp.end())
        {

            res.push_back(*it);
        }
        tmp[*it] = 1;
    }

    for (auto it = s2.begin(); it != s2.end(); it++)
    {

        if (tmp.find(*it) == tmp.end())
        {

            res.push_back(*it);
        }

        tmp[*it] = 1;
    }

    return res;
}

bool IsEqual(map<char, vector<char>> s1, map<char, vector<char>> s2)
{

    if (s1.size() == s2.size())
    {

        map<char, vector<char>>::iterator it;

        for (auto it = s1.begin(); it != s1.end(); it++)
        {

            if (it->second.size() != s2[it->first].size())

                return false;
        }

        return true;
    }

    return false;
}

vector<char> removeNull(vector<char> list)
{

    int index = -1;

    for (int i = 0; i < list.size(); i++)
    {

        if (list[i] == '~')
            index = i;
    }

    if (index == -1)
        return list;
    else
    {

        list.erase(list.begin() + index);

        return list;
    }
}

bool findFollow(map<char, vector<char>> &followSet, map<char, vector<char>> set1, map<char, vector<string>> context_free_grammar, unordered_map<char, int> nullset)
{

    map<char, vector<char>> newSet;

    for (auto it1 = context_free_grammar.begin(); it1 != context_free_grammar.end(); it1++)
    {

        vector<char> setTemp = followSet[it1->first];
        for (auto it = context_free_grammar.begin(); it != context_free_grammar.end(); it++)
        {

            for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++)
            {

                string prod = *it2;
                bool check = false;
                for (int i = 0; i <= prod.size(); i++)
                {

                    if (i == prod.size() && check)
                    {

                        setTemp = Combine(setTemp, followSet[it->first]);
                        break;
                    }

                    if (prod[i] == it1->first)
                    {

                        check = true;
                    }
                    else if (check)
                    {

                        if (isTerminal(prod[i]))
                        {

                            vector<char> single;
                            single.push_back(prod[i]);
                            setTemp = Combine(setTemp, single);
                            check = false;
                            break;
                        }
                        else
                        {

                            setTemp = Combine(setTemp, removeNull(set1[prod[i]]));

                            if (nullset.find(prod[i]) == nullset.end())
                            {
                                check = false;
                                break;
                            }
                        }
                    }
                }
            }
        }

        newSet[it1->first] = setTemp;
    }

    if (!IsEqual(newSet, followSet))
    {

        followSet = newSet;
        return false;
    }

    return true;
}

bool findFirst(map<char, vector<char>> &set1, map<char, vector<string>> context_free_grammar, unordered_map<char, int> nullset)
{

    bool check = false;

    map<char, vector<char>> newSet;

    for (auto it = context_free_grammar.begin(); it != context_free_grammar.end(); it++)
    {

        vector<char> prev, setTemp = set1[it->first];

        for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++)
        {

            string prod = *it2;
            for (int i = 0; i < prod.size(); i++)
            {

                if (prod[i] == '~')
                    continue;

                else if (isTerminal(prod[i]))
                {

                    vector<char> single;
                    single.push_back(prod[i]);
                    setTemp = Combine(setTemp, single);
                    break;
                }

                else
                {

                    setTemp = Combine(setTemp, set1[prod[i]]);

                    if (nullset.find(prod[i]) == nullset.end())
                        break;
                }
            }

            setTemp = Combine(setTemp, prev);
            prev = setTemp;
        }

        newSet[it->first] = setTemp;
    }

    if (!IsEqual(newSet, set1))
    {

        set1 = newSet;
        return false;
    }

    return true;
}

void find_all_null_productions(map<char, vector<char>> &set1, unordered_map<char, int> &nullset, map<char, vector<string>> context_free_grammar)
{

    map<char, vector<string>>::iterator it;
    vector<string>::iterator it2;

    bool check = true;
    while (check)
    {
        check = false;
        for (it = context_free_grammar.begin(); it != context_free_grammar.end(); it++)
        {

            if (nullset.find(it->first) == nullset.end())
            {

                for (it2 = it->second.begin(); it2 != it->second.end(); it2++)
                {

                    string prod = *it2;
                    bool check2 = true;
                    for (int i = 0; i < prod.size(); i++)
                    {

                        if (nullset.find(prod[i]) == nullset.end())
                        {

                            check2 = false;
                            break;
                        }
                    }

                    if (check2)
                    {
                        nullset[it->first] = 1;
                        set1[it->first].push_back('~');
                        check = true;
                        break;
                    }
                }
            }
        }
    }
}

vector<char> findliterals(map<char, vector<char>> set1, map<char, vector<char>> followSet, unordered_map<char, int> nullset, char p, string prod)
{
    vector<char> res;

    if (prod[0] == '~')
    {
        res = followSet[p];
        return res;
    }

    int cnt = 0;
    for (int i = 0; i < prod.size(); i++)
    {

        if (isTerminal(prod[i]))
        {
            vector<char> temp;
            temp.push_back(prod[i]);
            cnt = 1;
            res = Combine(res, temp);
            break;
        }

        res = Combine(res, set1[prod[i]]);

        if (find(set1[prod[i]].begin(), set1[prod[i]].end(), '~') == set1[prod[i]].end())
        {

            cnt = 1;
            break;
        }
    }

    if (!cnt)
        res = Combine(res, followSet[p]);

    return res;
}
void createLLTable(map<char, vector<char>> set1, map<char, vector<char>> followSet, unordered_map<char, int> nullset, map<char, vector<string>> context_free_grammar)
{

    map<pair<char, char>, string> ParseTable;
    mpt['$'] = 1;

    cout << "\n Part 3 \n";
    for (auto it1 = nonmpt.begin(); it1 != nonmpt.end(); it1++)
    {

        for (auto it2 = mpt.begin(); it2 != mpt.end(); it2++)
        {

            ParseTable[make_pair(it1->first, it2->first)] = "";
        }
    }

    for (auto it = context_free_grammar.begin(); it != context_free_grammar.end(); it++)
    {
        for (auto it3 = it->second.begin(); it3 != it->second.end(); it3++)
        {

            vector<char> setChar = findliterals(set1, followSet, nullset, it->first, *it3);

            for (int i = 0; i < setChar.size(); i++)
            {

                if (setChar[i] == '~')
                    continue;

                if (ParseTable[make_pair(it->first, setChar[i])].size() == 0)
                {
                    ParseTable[make_pair(it->first, setChar[i])] = *it3;
                }
                else
                {       
                    cout << "The given grammar is not LL(1) grammar" << endl;
                    cout << "LL1 parsing failed because of conflict with production: " << it->first << "=" << setChar[i] << endl;
                    return;
                }
            }
        }
    }
    cout << "Parse Table :-\n";
    cout << " \t\t";
    for (auto x : mpt)
        cout << x.first << "\t\t";
    cout << '\n';
    for (auto x : nonmpt)
    {
        cout << x.first << "\t\t";
        for (auto y : mpt)
        {
            if (ParseTable[{x.first, y.first}] != "")
            {
                cout << x.first << "=" << ParseTable[{x.first, y.first}];
            }

            cout << "\t\t";
        }
        cout << '\n';
    }
    cout << "\n Part 4 \n";
    cout << "The given grammar is LL(1) grammar\n";
}

class Productions
{
public:
    string variable;
    vector<string> productions;
    bool is_left_refactored;
    static vector<string> unused_variable_names;

    Productions(void)
    {
        is_left_refactored = false;
    }

    string complete_production(void)
    {
        string joined_productions = variable + " -> ";
        for (vector<string>::iterator pr = productions.begin(); pr != productions.end(); ++pr)
        {
            joined_productions += (*pr);
            if (pr != --productions.end())
            {
                joined_productions += " | ";
            }
        }
        return joined_productions;
    }
};
vector<string> Productions::unused_variable_names;

void do_left_factoring(vector<Productions> &all_productions)
{
    vector<Productions> new_prods;
    for (vector<Productions>::iterator it = all_productions.begin(); it != all_productions.end(); ++it)
    {
        if ((*it).is_left_refactored == 0)
        {
            // Check if EPSILON is there
            if ((*it).productions.size() == 1)
            {
                (*it).is_left_refactored = true;
                continue;
            }

            // Finding common slices
            sort((*it).productions.begin(), (*it).productions.end());
            int production_size = (*it).productions.size();
            vector<pair<int, int>> common_section_slices;
            for (int index = 0; index < production_size; index++)
            {
                int go_ahead = index;
                while (go_ahead < production_size)
                {
                    if ((*it).productions[index][0] == (*it).productions[go_ahead][0])
                        go_ahead++;
                    else
                        break;
                }
                go_ahead = go_ahead - 1;
                if (go_ahead != index)
                {
                    common_section_slices.push_back(make_pair(index, go_ahead));
                }
                index += (go_ahead - index);
            }

            if (common_section_slices.size() > 0)
            {
                cout << "\tFound left refactoring production: " << (*it).complete_production() << endl;
            }
            else
            {
                (*it).is_left_refactored = true; // Done refacoring
                continue;
            }

            for (vector<pair<int, int>>::iterator ptr = common_section_slices.begin(); ptr != common_section_slices.end(); ptr++)
            {
                // Find the longest common substring
                int index = 0;
                int small_length = min((*it).productions[(*ptr).first].size(), (*it).productions[(*ptr).second].size());
                while (index < small_length && (*it).productions[(*ptr).first][index] == (*it).productions[(*ptr).second][index])
                    index++;

                string pre = (*it).productions[(*ptr).first].substr(0, index);
                cout << "\t\tFound common substring: " << pre << endl;

                Productions variable_prime;
                variable_prime.variable = Productions::unused_variable_names[Productions::unused_variable_names.size() - 1];
                Productions::unused_variable_names.pop_back();
                for (vector<string>::iterator split_prod = (*it).productions.begin() + (*ptr).first; split_prod <= (*it).productions.begin() + (*ptr).second; split_prod++)
                {
                    string new_names;
                    if (index == (*split_prod).size()) //  Fully exhausted
                        new_names = "~";
                    else
                        new_names = (*split_prod).substr(index, (*split_prod).size());
                    variable_prime.productions.push_back(new_names);

                    // Replace original with variable_prime.variable
                    (*split_prod).replace(index, (*split_prod).size(), variable_prime.variable);
                }

                (*it).productions[(*ptr).first] = pre + variable_prime.variable;
                for (int itr_int = (*ptr).second; itr_int > (*ptr).first; itr_int--)
                {
                    (*it).productions.erase((*it).productions.begin() + itr_int);
                }

                int slice_length = (*ptr).second - (*ptr).first;
                for (vector<pair<int, int>>::iterator leap_ahead = ptr; leap_ahead != common_section_slices.end(); leap_ahead++)
                {
                    (*leap_ahead).first -= slice_length;
                    (*leap_ahead).second -= slice_length;
                }

                cout << "\t\tNew Productions created: " << variable_prime.complete_production() << endl;
                cout << "\t\tRefactored Productions: " << (*it).complete_production() << endl;
                new_prods.push_back(variable_prime);
            }
            (*it).is_left_refactored = true;
        }
    }

    for (vector<Productions>::iterator new_prod = new_prods.begin(); new_prod != new_prods.end(); new_prod++)
    {
        all_productions.push_back((*new_prod));
    }
}

int main()
{
    // Setting up all unused variables
    for (char var = 'A'; var <= 'Z'; var++)
    {
        Productions::unused_variable_names.push_back(string(1, var));
    }

    vector<Productions> all_productions;
    int ntoken, vtoken;
    ntoken = yylex();
    while (ntoken)
    {
        cout << ntoken << " " << yytext << endl;
        Productions new_production;
        new_production.variable = yytext;
        vector<string>::iterator var_location = find(Productions::unused_variable_names.begin(), Productions::unused_variable_names.end(), new_production.variable);
        Productions::unused_variable_names.erase(var_location);
        ntoken = yylex();
        while (ntoken != NEW_LINE)
        {
            if (ntoken == PRODUCTIONS or ntoken == VARIABLE)
            {
                new_production.productions.push_back(yytext);
            }
            ntoken = yylex();
        }
        ntoken = yylex();
        all_productions.push_back(new_production);
    }

    // Printing all Productions
    cout << "\nParsing" << endl;
    for (vector<Productions>::iterator it = all_productions.begin(); it != all_productions.end(); ++it)
        cout << "\tProduction: " << (*it).complete_production() << endl;

    cout << "\n Part 1 \n";
    // Removing the left recursion
    cout << "\nRemoving left recursion" << endl;
    vector<Productions> new_productions;
    for (vector<Productions>::iterator it = all_productions.begin(); it != all_productions.end(); ++it)
    {
        bool is_left_recursive = false;
        for (vector<string>::iterator pr = (*it).productions.begin(); pr != (*it).productions.end(); ++pr)
        {
            if ((*it).variable[0] == (*pr)[0])
            {
                is_left_recursive = true;
                break;
            }
        }

        if (is_left_recursive == true)
        {
            cout << "\tFound left recursive production: " << (*it).complete_production() << endl;

            // Separate out left and non left production before splitting
            vector<string> alphas;
            vector<string> betas;
            for (vector<string>::iterator pr = (*it).productions.begin(); pr != (*it).productions.end(); ++pr)
            {
                if ((*it).variable[0] == (*pr)[0]) // left
                {
                    alphas.push_back((*pr).substr(1, (*pr).size()));
                }
                else
                {
                    betas.push_back((*pr));
                }
            }

            if (betas.size() == 0)
            {
                cout << "\tRemoving left recursion fails because there are no non-left productions (betas) for ";
                cout << (*it).complete_production() << endl;
                return 1;
            }

            Productions variable_prime;
            variable_prime.variable = Productions::unused_variable_names[Productions::unused_variable_names.size() - 1];
            Productions::unused_variable_names.pop_back();
            // Adjusting left recursion
            (*it).productions.clear();
            for (vector<string>::iterator pr = betas.begin(); pr != betas.end(); ++pr)
            {
                string new_prod = (*pr) + variable_prime.variable;
                (*it).productions.push_back(new_prod);
            }
            // Creating new prime productions
            for (vector<string>::iterator pr = alphas.begin(); pr != alphas.end(); ++pr)
            {
                string new_prod = (*pr) + variable_prime.variable;
                variable_prime.productions.push_back(new_prod);
            }
            variable_prime.productions.push_back("~");
            new_productions.push_back(variable_prime);
        }
    }
    for (vector<Productions>::iterator it = new_productions.begin(); it != new_productions.end(); ++it)
    {
        all_productions.push_back((*it));
    }

    // Printing all Productions
    cout << "\nAfter removing left recursions" << endl;
    for (vector<Productions>::iterator it = all_productions.begin(); it != all_productions.end(); ++it)
        cout << "\tProduction: " << (*it).complete_production() << endl;

    // Removing Left Factoring
    cout << "\n Part 2 \n";
    cout << "\nRemoving Left Factoring" << endl;
    int batch_factoring = 1;
    while (true)
    {
        cout << "Round " << batch_factoring << " of refactoring -->\n";
        do_left_factoring(all_productions);
        bool all_done = true;
        for (vector<Productions>::iterator check_left = all_productions.begin(); check_left != all_productions.end(); check_left++)
        {
            if ((*check_left).is_left_refactored == 0)
            {
                all_done = false;
                break;
            }
        }
        batch_factoring++;
        if (all_done == 1)
        {
            cout << "\tRefactoring done\n";
            break;
        }
    }

    cout << "\nProductions at hand" << endl;
    for (vector<Productions>::iterator it = all_productions.begin(); it != all_productions.end(); ++it)
        cout << "\tProduction: " << (*it).complete_production() << endl;

    map<char, vector<char>> set1, followSet;
    unordered_map<char, int> nullset;
    map<char, vector<string>> context_free_grammar, purecontext_free_grammar;

    int index = 0;
    for (vector<Productions>::iterator it = all_productions.begin(); it != all_productions.end(); ++it)
    {
        char c = (*it).variable[0];
        context_free_grammar[c] = vector<string>();
        context_free_grammar[c] = vector<string>();
        set1[c] = vector<char>();
        followSet[c] = vector<char>();
        if (index++ == 0)
            followSet[c].push_back('$');

        nonmpt[c] = 1;

        for (vector<string>::iterator ptr = (*it).productions.begin(); ptr != (*it).productions.end(); ptr++)
        {
            purecontext_free_grammar[c].push_back((*ptr));
            if ((*ptr).compare("~") == 0)
            {
                nullset[c] = 1;
                set1[c].push_back('~');
                continue;
            }
            context_free_grammar[c].push_back((*ptr));
        }
    }

    cout << "***********************************************\n" << endl;
    findmpt(context_free_grammar);

    find_all_null_productions(set1, nullset, context_free_grammar);
    while (true)
    {

        if (findFirst(set1, context_free_grammar, nullset))
        {

            break;
        }
    }

    cout << "First Set :- " << endl;
    pretty_print(set1);

    cout << "***********************************************\n";

    while (true)
    {

        if (findFollow(followSet, set1, context_free_grammar, nullset))
        {

            break;
        }
    }

    cout << "Follow Set :- " << endl;
    pretty_print(followSet);

    cout << "***********************************************\n";

    createLLTable(set1, followSet, nullset, purecontext_free_grammar);

    cout << "\n--------------------------------------------------\n";
    return 0;
}
