#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <sstream>
#include <queue>
#include <stdlib.h>
#include <limits.h>
const int M = 1000; // order of the B-tree
using namespace std;
struct Property
{
    string name;
    string location;
    float price;
    string amenities;
    float num_bedrooms;
    
};
struct ElementComparator
{
    bool operator()(const Property &lhs, const Property &rhs) const
    {
        return lhs.name < rhs.name;
    }
};
struct SearchResult
{
    Property e;
    int total_score;
};
struct sr
{
    int id;
    float price, rating;
};
struct Node
{
    vector<Property> elements;
    vector<Node *> children;
};
// function to search for an element in a B-tree
struct cmp
{
    bool operator()(const SearchResult &lhs, const SearchResult &rhs)
    {
        return lhs.total_score < rhs.total_score;
    }
};
struct node
{
    float min_price;
    float max_price;
    vector<sr> elements;
    node *left;
    node *right;
};
struct node2
{
    float min_rooms;
    float max_rooms;
    vector<sr> elements2;
    node2 *left;
    node2 *right;
};
vector<SearchResult> searchHelper(Node *node, const string &query)
{
    vector<SearchResult> results;
    int i = 0;
    string lq = query;
    transform(lq.begin(), lq.end(), lq.begin(), ::tolower);
    vector<string> queryWords;
    stringstream ss(lq);
    string word;
    while (ss >> word)
    {
        queryWords.push_back(word);
    }
    while (i < node->elements.size())
    {
        int score = 0;
        string ln = node->elements[i].name;
        transform(ln.begin(), ln.end(), ln.begin(), ::tolower);
        string ld = node->elements[i].amenities;
        transform(ld.begin(), ld.end(), ld.begin(), ::tolower);
        string lc = node->elements[i].location;
        transform(lc.begin(), lc.end(), lc.begin(), ::tolower);
        int queryWordIndex = 0;
        for (auto &queryWord : queryWords)
        {
            if (ln.find(lq) != string::npos)
            {
                struct SearchResult result;
                result.e = node->elements[i];
                result.total_score = 3;
                results.push_back(result);
                score += 3;
            }
            else if (lc.find(lq) != string::npos)
            {
                struct SearchResult result;
                result.e = node->elements[i];
                result.total_score = 2;
                results.push_back(result);
                score += 2;
            }
            else if (ld.find(lq) != string::npos)
            {
                struct SearchResult result;
                result.e = node->elements[i];
                result.total_score = 1;
                results.push_back(result);
                score++;
            }
            queryWordIndex++;
        }
        i++;
    }
    if (!node->children.empty())
    {
        if (i == node->elements.size() || query <= node->elements[i].name)
        {
            vector<SearchResult> results;
            results = searchHelper(node->children[i], query);
        }
        if (i == 0 || query > node->elements[i].name)
        {
            vector<SearchResult> results;
            results = searchHelper(node->children[i + 1], query);
        }
    }
    priority_queue<SearchResult, vector<SearchResult>, cmp> pq;
    for (auto &result : results)
    {
        // pq.push({result, 0});
        pq.push(result);
        if (pq.size() > 10)
        {
            pq.pop();
        }
    }
    vector<SearchResult> topResults;
    while (!pq.empty())
    {
        topResults.push_back(pq.top());
        pq.pop();
    }
    return topResults;
}
vector<SearchResult> search(Node *node, const string &query, vector<Property> &results)
{
    istringstream iss(query);
    string word;
    vector<SearchResult> res;
    vector<SearchResult> r;
    while (iss >> word)
    {
        r = searchHelper(node, word);
        vector<SearchResult> r_new = searchHelper(node, word);
        r.insert(r.end(), r_new.begin(), r_new.end());
    }
    set<Property, ElementComparator> seen;
    for (int i = 0; i < r.size(); i++)
    {
        if (seen.find(r[i].e) == seen.end())
        {
            res.push_back(r[i]);
            seen.insert(r[i].e);
        }
    }
    return res;
}

void insert(Node *&node, const Property &element)
{
    if (node == nullptr)
    {
        node = new Node;
        node->elements.push_back(element);
        return;
    }
    int i = 0;
    while (i < node->elements.size() && element.name > node->elements[i].name)
    {
        i++;
    }
    if (i < node->elements.size() && element.name == node->elements[i].name)
    {
        // element already exists in the tree
        return;
    }
    if (node->children.empty())
    {
        // leaf node
        node->elements.insert(node->elements.begin() + i, element);
        if (node->elements.size() > M)
        {
            // split the node
            Node *new_node = new Node;
            new_node->elements = vector<Property>(node->elements.begin() + M / 2, node->elements.end());
            node->elements.resize(M / 2);
            if (node == nullptr)
            {
                node = new Node;
                node->elements.push_back(new_node->elements[0]);
                node->children.push_back(new_node);
            }
            else
            {
                node->children.insert(node->children.begin() + i + 1, new_node);
            }
        }
    }
    else
    {
        // non-leaf node
        insert(node->children[i], element);
        if (node->children[i]->elements.size() > M)
        {
            // split the child node
            Node *new_node = new Node;
            new_node->elements = vector<Property>(node->children[i]->elements.begin() + M / 2, node->children[i]->elements.end());
            node->children[i]->elements.resize(M / 2);
            node->children.insert(node->children.begin() + i + 1, new_node);
            node->elements.insert(node->elements.begin() + i, node->children[i]->elements.back());
            node->children[i]->elements.pop_back();
            if (node->elements.size() > M)
            {
                // split the node
                Node *new_root = new Node;
                new_root->elements = vector<Property>(node->elements.begin() + M / 2, node->elements.end());
                new_root->children = vector<Node *>(node->children.begin() + M / 2 + 1, node->children.end());
                node->elements.resize(M / 2);
                node = new_root;
            }
        }
    }
}
std::string SearchResultToJson(const std::vector<SearchResult> &results)
{
    std::stringstream ss;
    ss << "[\n";
    for (size_t i = 0; i < results.size(); ++i)
    {
        const Property &p = results[i].e;
        ss << "  {\n"
           << "    \"element\": {\n"
           << "      \"Location\": \"" << p.location << "\",\n"
           << "      \"Name\": \"" << p.name << "\",\n"
           << "      \"Rent per month\": " << p.price << ",\n"
           << "      \"Amenities\": \"" << p.amenities << "\",\n"
           << "      \"Number of Bedrooms\": " << p.num_bedrooms << "\n"
           << "    },\n"
           << "    \"total_score\": " << results[i].total_score << "\n"
           << "  }";
        if (i != results.size() - 1)
        {
            ss << ",";
        }
        ss << "\n";
    }
    ss << "]";
    return ss.str();
}
node *builp(vector<sr> &elements, int start, int end)
{
    if (start == end)
    {
        node *leaf = new node{elements[start].price, elements[start].price, {elements[start]}, nullptr, nullptr};
        return leaf;
    }
    int mid = start + (end - start) / 2;
    node *left_chilp = builp(elements, start, mid);
    node *right_chilp = builp(elements, mid + 1, end);
    node *parent = new node{min(left_chilp->min_price, right_chilp->min_price),
                            max(left_chilp->max_price, right_chilp->max_price),
                            {},
                            left_chilp,
                            right_chilp};
    parent->elements.reserve(left_chilp->elements.size() + right_chilp->elements.size());
    parent->elements.insert(parent->elements.end(), left_chilp->elements.begin(), left_chilp->elements.end());
    parent->elements.insert(parent->elements.end(), right_chilp->elements.begin(), right_chilp->elements.end());
    return parent;
}

vector<sr> sr_query_2(node2 *node2, float min_rooms, float max_rooms)
{
    if (node2 == nullptr)
    {
        return {};
    }
    if (node2->min_rooms > max_rooms || node2->max_rooms < min_rooms)
    {
        return {};
    }
    if (node2->min_rooms >= min_rooms && node2->max_rooms <= max_rooms)
    {
        return node2->elements2;
    }
    vector<sr> left_results = sr_query_2(node2->left, min_rooms, max_rooms);
    vector<sr> right_results = sr_query_2(node2->right, min_rooms, max_rooms);
    vector<sr> results;
    results.reserve(left_results.size() + right_results.size());
    results.insert(results.end(), left_results.begin(), left_results.end());
    results.insert(results.end(), right_results.begin(), right_results.end());
    return results;
}
node2 *builp_2(vector<sr> &elements2, int start, int end)
{
    if (start == end)
    {
        node2 *leaf = new node2{elements2[start].rating, elements2[start].rating, {elements2[start]}, nullptr, nullptr};
        return leaf;
    }
    int mid = start + (end - start) / 2;
    node2 *left_chilp = builp_2(elements2, start, mid);
    node2 *right_chilp = builp_2(elements2, mid + 1, end);
    node2 *parent = new node2{min(left_chilp->min_rooms, right_chilp->min_rooms),
                              max(left_chilp->max_rooms, right_chilp->max_rooms),
                              {},
                              left_chilp,
                              right_chilp};
    parent->elements2.reserve(left_chilp->elements2.size() + right_chilp->elements2.size());
    parent->elements2.insert(parent->elements2.end(), left_chilp->elements2.begin(), left_chilp->elements2.end());
    parent->elements2.insert(parent->elements2.end(), right_chilp->elements2.begin(), right_chilp->elements2.end());
    return parent;
}
vector<sr> sr_query(node *node, float min_price, float max_price)
{
    if (node == nullptr)
    {
        return {};
    }
    if (node->min_price > max_price || node->max_price < min_price)
    {
        return {};
    }
    if (node->min_price >= min_price && node->max_price <= max_price)
    {
        return node->elements;
    }
    vector<sr> left_results = sr_query(node->left, min_price, max_price);
    vector<sr> right_results = sr_query(node->right, min_price, max_price);
    vector<sr> results;
    results.reserve(left_results.size() + right_results.size());
    results.insert(results.end(), left_results.begin(), left_results.end());
    results.insert(results.end(), right_results.begin(), right_results.end());
    return results;
}
int main(void)
{
    // if (argc != 6)
    // {
    //     cout << "Usage: " << argv[0] << " <input_file>" << endl;
    //     return 1;
    // }
    while(1){
    cout<<"Enter details"<<endl;
    string name;
    transform(name.begin(),name.end(),name.begin(),::tolower);
    float max_p,min_p,max_b,min_b;
    cin>>name>>min_p>>max_p>>min_b>>max_b;
    if(min_p>900000||max_p<10000||min_b>4||max_b<1){
        cout<<"No property listings for the given inputs...try again."<<endl;
    }
    else{
    Node *root = nullptr;
    vector<Property> places;
    //locations in chennai
    places.push_back({ "Pacifica Aurum, Duplex", "Chennai", 50000,"Wifi, Clean water, Ample sunlight, Natural breeze",2 });
    places.push_back({ "Hira Nandani, Condominium", "Chennai", 260000,"Clubhouse for all activities, Wifi, Parking, Balcony, Housekeeping",4 });
    places.push_back({ "Bier Garten, Apartment", "Chennai", 120000,"Wifi, Clean water, Gymnasium, Housekeeping",3 });
    places.push_back({ "Lalit Ashok, Studio", "Chennai", 84000,"Wifi, Clean water, Balcony, Housekeeping",2 });
    places.push_back({ "Adarsh Vista, Loft ", "Chennai", 500000,"Wifi, Clean water, Kitchen",1 });
    places.push_back({ "Altezza, Apartment", "Chennai", 70000,"Wifi, Clean water, Ample sunlight, Natural breeze, Gymnasium, Spa",3 });
    places.push_back({ "Rainbow Drive, Studio", "Chennai", 10000,"Clean water, Balcony",1 });
    places.push_back({ "Ashiana, Apartment", "Chennai", 49000,"Wifi, Clean water, Spa, Clubhouse",3 });
    places.push_back({ "Purvankar Residency, Alcove Studio ", "Chennai", 20000,"Wifi, Clean water, Balcony, Sauna",2 });
    places.push_back({ "Golden Blossom, Condominium", "Chennai", 92000,"Wifi, Clean water, Gymnasium",3 });

    //locations in bangalore
    places.push_back({ "Mantri Elite, Duplex", "Bangalore", 75000,"Wifi, Clean water, Ample sunlight, Natural breeze",2 });
    places.push_back({ "Windsor Gardenia, Condominium", "Bangalore", 120000,"Clubhouse for all activities, Wifi, Parking, Balcony, Housekeeping",4 });
    places.push_back({ "Kensington Towers, High-Rise", "Bangalore", 55000,"Wifi, Clean water, Hospital, Pool, Gymnasium",2 });
    places.push_back({ "Adarsh Sanctuary, Low-Rise", "Bangalore", 83000,"Wifi, Clean water, Gymnasium, Housekeeping",2 });
    places.push_back({ "Olympia Sky, Apartment", "Bangalore", 140000,"Wifi, Clean water, Hospital, Housekeeping",3 });
    places.push_back({ "Sreedevi Residency, Studio", "Bangalore", 59000,"Wifi, Clean water, Ample sunlight, Hospital",2 });
    places.push_back({ "7 seasons, High-Rise", "Bangalore", 94000,"Wifi, Clean water, Supermarket, Gymnasium",3 });
    places.push_back({ "Peshavar Residency, Basement", "Bangalore", 33000,"Wifi, Clean water, Hospital, Supermarket",1 });
    places.push_back({ "Silver Blossom, Loft", "Bangalore", 12000,"Wifi, Clean water, Kitchen",1 });
    places.push_back({ "Tulip Gardens, Condominium", "Bangalore", 200000,"Wifi, Clean water, Balcony, Housekeeping",3 });

    //locations in Pune
    places.push_back({ "Silversands, Alcove studio", "Pune", 34000,"Wifi, Clean water, Ample sunlight, Natural breeze",1 });
    places.push_back({ "Mundhwa Pune, Condominium", "Pune", 145000,"Clubhouse for all activities, Wifi, Parking, Balcony, Housekeeping",3 });
    places.push_back({ "Shreyas Shri Vaari, High-Rise", "Pune", 250000,"Wifi, Clean water, Hospital, Pool, Gymnasium",4 });
    places.push_back({ "Kohinoor Presidentia, High-Rise", "Pune", 79000,"Wifi, Clean water, Gymnasium, Housekeeping",2 });
    places.push_back({ "Pristine O2, Low-Rise", "Pune", 410000,"Wifi, Clean water, Hospital, Housekeeping",3 });
    places.push_back({ "Mahindra Association, Basement", "Pune", 23000,"Wifi, Clean water, Hospital",1 });
    places.push_back({ "Anandtara, Duplex", "Pune", 16000,"Wifi, Clean water, Supermarket, Gymnasium, Housekeeping",2 });
    places.push_back({ "Merlin Ventana, Low-Rise", "Pune", 47000,"Wifi, Clean water, Supermarket",2 });
    places.push_back({ "Pride montreal, Condominium", "Pune", 55000,"Wifi, Clean water, Kitchen, Clubhouse, Supermarket",4 });
    places.push_back({ "Mantra Magnus, Loft", "Pune", 49000,"Wifi, Clean water, Balcony",1 });

    //locations in trichy
    places.push_back({ "Rohini Colours, Alcove studio", "Trichy", 94000,"Wifi, Clean water, Ample sunlight, Natural breeze",3 });
    places.push_back({ "Mundhwa Trichy, Condominium", "Trichy", 160000,"Clubhouse for all activities, Wifi, Parking, Balcony, Housekeeping",3 });
    places.push_back({ "Royal Vesta, High-Rise", "Trichy", 15000,"Wifi, Clean water, Hospital, Pool, Gymnasium",1 });
    places.push_back({ "Vasan City, High-Rise", "Trichy", 49000,"Wifi, Clean water, Gymnasium, Housekeeping",2 });
    places.push_back({ "Bharat Green Empire, Low-Rise", "Trichy", 60000,"Wifi, Clean water, Hospital, Housekeeping",1 });
    places.push_back({ "Sakthi Renga, Basement", "Trichy", 90000,"Wifi, Clean water, Hospital",4 });
    places.push_back({ "Afraz, Duplex", "Trichy", 53000,"Wifi, Clean water, Supermarket, Gymnasium, Housekeeping",2 });
    places.push_back({ "Isha Aarambha, Low-Rise", "Trichy", 67000,"Wifi, Clean water, Supermarket",3 });
    places.push_back({ "Rengaraman, Condominium", "Trichy", 59000,"Wifi, Clean water, Kitchen, Clubhouse, Supermarket",4 });
    places.push_back({ "Priya Sudarshan, Loft", "Trichy", 48000,"Wifi, Clean water, Balcony",2 });

    //locations in delhi
    places.push_back({ "Som Vihar, Alcove studio", "Delhi", 250000,"Wifi, Clean water, Ample sunlight, Natural breeze",4 });
    places.push_back({ "Amaryllis, Condominium", "Delhi", 185000,"Clubhouse for all activities, Wifi, Parking, Balcony, Housekeeping",2 });
    places.push_back({ "DLF, High-Rise", "Delhi", 25000,"Wifi, Clean water, Hospital, Pool, Gymnasium",1 });
    places.push_back({ "Orchid Metropolis, High-Rise", "Delhi", 75000,"Wifi, Clean water, Gymnasium, Housekeeping",3 });
    places.push_back({ "Oberoi, Low-Rise", "Delhi", 88000,"Wifi, Clean water, Hospital, Housekeeping",3 });
    places.push_back({ "Akash Ganga, Basement", "Delhi", 19000,"Wifi, Clean water, Hospital",2 });
    places.push_back({ "Sarojini, Duplex", "Delhi", 34000,"Wifi, Clean water, Supermarket, Gymnasium, Housekeeping",2 });
    places.push_back({ "Nilgiri, Low-Rise", "Delhi", 49900,"Wifi, Clean water, Supermarket",3 });
    places.push_back({ "Sunrise, Condominium", "Delhi", 99000,"Wifi, Clean water, Kitchen, Clubhouse, Supermarket",4 });
    places.push_back({ "Triveni, Loft", "Delhi", 55000,"Wifi, Clean water, Balcony",2 });


     //locations in surat
    places.push_back({ "Meghsarman, Alcove studio", "Surat", 350000,"Wifi, Clean water, Ample sunlight, Natural breeze",3 });
    places.push_back({ "Santvan Newon, Condominium", "Surat", 78000,"Clubhouse for all activities, Wifi, Parking, Balcony, Housekeeping",3 });
    places.push_back({ "The Laurels, High-Rise", "Surat", 25000,"Wifi, Clean water, Hospital, Pool, Gymnasium",2 });
    places.push_back({ "Sangini Vedanta, High-Rise", "Surat", 18000,"Wifi, Clean water, Gymnasium, Housekeeping",1 });
    places.push_back({ "Ratan Jyoti, Low-Rise", "Surat", 480000,"Wifi, Clean water, Hospital, Housekeeping",3 });
    places.push_back({ "Suryarath, Basement", "Surat", 39000,"Wifi, Clean water, Hospital",2 });
    places.push_back({ "Sangini Nirvana, Duplex", "Surat", 165000,"Wifi, Clean water, Supermarket, Gymnasium, Housekeeping",4 });
    places.push_back({ "Solitaire, Low-Rise", "Surat", 49800,"Wifi, Clean water, Supermarket",3 });
    places.push_back({ "Sahaj Panoramic, Condominium", "Surat", 320000,"Wifi, Clean water, Kitchen, Clubhouse, Supermarket",4 });
    places.push_back({ "Maple Leaf, Loft", "Surat", 44000,"Wifi, Clean water, Balcony",1 });

    //locations in mumbai
    places.push_back({ "Blue Bell, Alcove studio", "Mumbai", 470000,"Wifi, Clean water, Ample sunlight, Natural breeze",2 });
    places.push_back({ "Kalpataru Bliss, Condominium", "Mumbai", 345000,"Clubhouse for all activities, Wifi, Parking, Balcony, Housekeeping",3 });
    places.push_back({ "Park Royale, High-Rise", "Mumbai", 67800,"Wifi, Clean water, Hospital, Pool, Gymnasium",3 });
    places.push_back({ "Mount Blanc, High-Rise", "Mumbai", 89400,"Wifi, Clean water, Gymnasium, Housekeeping",2 });
    places.push_back({ "Roswalt Zaiden, Low-Rise", "Mumbai", 416000,"Wifi, Clean water, Hospital, Housekeeping",4 });
    places.push_back({ "Avanti, Basement", "Mumbai", 30000,"Wifi, Clean water, Hospital",1 });
    places.push_back({ "Nellai, Duplex", "Mumbai", 19000,"Wifi, Clean water, Supermarket, Gymnasium, Housekeeping",2 });
    places.push_back({ "Trilok, Low-Rise", "Mumbai", 78900,"Wifi, Clean water, Supermarket",3 });
    places.push_back({ "Galaxy, Condominium", "Mumbai", 556000,"Wifi, Clean water, Kitchen, Clubhouse, Supermarket",4 });
    places.push_back({ "Taj, Loft", "Mumbai", 43400,"Wifi, Clean water, Balcony",1 });

    //locations in kolkata
    places.push_back({ "Geetanjali, Alcove studio", "Kolkata", 43000,"Wifi, Clean water, Ample sunlight, Natural breeze",1 });
    places.push_back({ "Orchard Amritaya, Condominium", "Kolkata", 845000,"Clubhouse for all activities, Wifi, Parking, Balcony, Housekeeping",3 });
    places.push_back({ "Suncity, High-Rise", "Kolkata", 294000,"Wifi, Clean water, Hospital, Pool, Gymnasium",3 });
    places.push_back({ "Laguna Bay, High-Rise", "Kolkata", 79800,"Wifi, Clean water, Gymnasium, Housekeeping",2 });
    places.push_back({ "Imperial Park, Low-Rise", "Kolkata", 22000,"Wifi, Clean water, Hospital, Housekeeping",1 });
    places.push_back({ "Exotica, Basement", "Kolkata", 99800,"Wifi, Clean water, Hospital",4 });
    places.push_back({ "South City LNT, Duplex", "Kolkata", 15000,"Wifi, Clean water, Supermarket, Gymnasium, Housekeeping",2 });
    places.push_back({ "Raintree, Low-Rise", "Kolkata", 68000,"Wifi, Clean water, Supermarket",3 });
    places.push_back({ "Nandan, Condominium", "Kolkata", 34000,"Wifi, Clean water, Kitchen, Clubhouse, Supermarket",2 });
    places.push_back({ "Ashoka Chakra, Loft", "Kolkata", 78000,"Wifi, Clean water, Balcony",1 });

    //locations in hyderabad
    places.push_back({ "Shangri La, Alcove studio", "Hyderabad", 343000,"Wifi, Clean water, Ample sunlight, Natural breeze",3 });
    places.push_back({ "Rajapushpa Cannon Dale, Condominium", "Hyderabad", 14000,"Clubhouse for all activities, Wifi, Parking, Balcony, Housekeeping",2 });
    places.push_back({ "Lanco Hills, High-Rise", "Hyderabad", 259000,"Wifi, Clean water, Hospital, Pool, Gymnasium",4 });
    places.push_back({ "Prestige High Fields, High-Rise", "Hyderabad", 33000,"Wifi, Clean water, Gymnasium, Housekeeping",1 });
    places.push_back({ "Krishe Valley, Low-Rise", "Hyderabad", 716000,"Wifi, Clean water, Hospital, Housekeeping",3 });
    places.push_back({ "Avalon, Basement", "Hyderabad", 29000,"Wifi, Clean water, Hospital",2 });
    places.push_back({ "Welkkin Park, Duplex", "Hyderabad", 17000,"Wifi, Clean water, Supermarket, Gymnasium, Housekeeping",2 });
    places.push_back({ "Phoenix Halcyon, Low-Rise", "Hyderabad", 89000,"Wifi, Clean water, Supermarket",1 });
    places.push_back({ "Aditya Beaumont, Condominium", "Hyderabad", 555000,"Wifi, Clean water, Kitchen, Clubhouse, Supermarket",4 });
    places.push_back({ "Vishnu Blue Lotus, Loft", "Hyderabad", 78900,"Wifi, Clean water, Balcony",3 });



    for (int i = 0; i < places.size(); i++)
    {
        insert(root, places[i]);
    }
    vector<Property> result;
    vector<SearchResult> r;
    r = search(root, name, result);
    vector<sr> sr_v;
    for (int i = 0; i < r.size(); i++)
    {
        sr_v.push_back({i, r[i].e.price, r[i].e.num_bedrooms});
    }
    node *rt = builp(sr_v, 0, sr_v.size() - 1);
    float min_price = min_p;
    float max_price = max_p;
    vector<sr> results = sr_query(rt, min_price, max_price);
    vector<SearchResult> fsr;
    for (const sr &e : results)
    {
        fsr.push_back(r[e.id]);
    }
    vector<sr> sr_v_f;
    for (int i = 0; i < fsr.size(); i++)
    {
        sr_v_f.push_back({i, fsr[i].e.price, fsr[i].e.num_bedrooms});
    }
    node2 *rt2 = builp_2(sr_v_f, 0, sr_v_f.size() - 1);
    float min_rooms = min_b;
    float max_rooms = max_b;
    vector<sr> results2 = sr_query_2(rt2, min_rooms, max_rooms);
    vector<SearchResult> fsr_f;
    for (const sr &e : results2)
    {
        fsr_f.push_back(fsr[e.id]);
    }
    string finalResult = SearchResultToJson(fsr);
    cout << finalResult << endl;
    break;
     }
    }
}
