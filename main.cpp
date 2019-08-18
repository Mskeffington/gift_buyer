#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include <memory>
#include <random>
using namespace std;

struct Person {
public:
    string name;
    vector<shared_ptr<Person>> eligableBuyers;
    int group = 0;
    Person(string n, int groupId) { name = n; group = groupId;}
};

struct compare_remaining_buyers {
    bool operator()(const shared_ptr<Person> &lhs, const shared_ptr<Person> &rhs) const {
        return lhs->eligableBuyers.size() < rhs->eligableBuyers.size();
    }
};

vector<shared_ptr<Person>> createPeople(const vector<vector<string>>& all) {
    vector<shared_ptr<Person>> allPeople;
    int groupId = 0;
    
    // init all person objects
    for (auto group : all) {
        for (auto name : group) {
            shared_ptr<Person> person = make_shared<Person>(name, groupId);
            allPeople.push_back(person);
        }
        groupId++;
    }
    
    // assign each person a list of valid partners.  ie: anyone not in their group
    for (auto person : allPeople) {
        for (auto personRecip : allPeople) {
            if (personRecip->group != person->group)
                person->eligableBuyers.push_back(personRecip);
        }
    }
    
    return allPeople;
}

map<string, string> createPairs(const vector<shared_ptr<Person>>& constpeople) {
    vector<shared_ptr<Person>> people(constpeople.begin(), constpeople.end());
    map<string, string> pairings;
    
    random_device r;
    default_random_engine e1(r());
    
    while (!people.empty()) {
        // Start with person with least choices for partners
        sort(people.begin(), people.end(), compare_remaining_buyers());
        auto person = people.at(0);
        
        // if any person has 0 partner options, we fucked up
        if (person->eligableBuyers.empty()) {
            cerr << "Error at " << person->name << endl;
            return pairings;
        }
                
        // assign a random partner from valid list
        uniform_int_distribution<int> distribution (0, person->eligableBuyers.size()-1);
        auto partner = person->eligableBuyers.at(distribution(e1));
        
        // remove chosen person from all valid recipiant lists
        for (auto p : people) {
            for (auto itr = p->eligableBuyers.begin(); itr != p->eligableBuyers.end(); itr++) {
                if (*itr == partner) {
                    p->eligableBuyers.erase(itr);
                    break;
                }
            }
        }
        pairings[partner->name] = person->name;
        people.erase(people.begin());
    }
    return pairings;
}

void printList(const map<string, string>& pairings) {
    cout << "Buyer\tReceiver" << endl << "------------------------" << endl;
    for (auto pairingTuple : pairings) 
        cout << get<0>(pairingTuple) << "\t" << get<1>(pairingTuple) << endl;
}

int main(int argc, char **argv) {
    vector<string> group1 = {"Mike", "Katie", "Diane", "Rick"};
    vector<string> group2 = {"Ruth", "Bob", "Linda", "John", "Karan", "Larry"};
    vector<string> group3 = {"Ceil", "Bobby", "Jen"};
    vector<string> group4 = {"Jean"};
    vector<string> group5 = {"Denise", "George"};
    vector<vector<string>> all = {group1, group2, group3, group4, group5};
    
    auto people = createPeople(all);
    map<string, string> pairings = createPairs(people);
    
    printList(pairings);
    if (pairings.size() != people.size()) {
        for (auto pairingTuple : pairings) 
            cerr << get<0>(pairingTuple) << " - " << get<1>(pairingTuple) << endl;
        cerr << "Something messed up!  Stuck with no valid pairings." << endl;
        return 1;
    }
    
    return 0;
}
