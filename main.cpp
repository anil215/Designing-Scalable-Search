#include<bits/stdc++.h>
using namespace std;
# define THRESHOLD 500
class Property
{
    private:
        int propertyId, latitude, longitude, price, bedroom, bathroom, category;
    public:
        Property(int propertyId,int latitude,int longitude,int price,int bedroom,int bathroom,int category)
            :propertyId(propertyId),latitude(latitude),longitude(longitude),price(price),bedroom(bedroom),
            bathroom(bathroom),category(category)
        {

        }
        void get_details()
        {
            cout<<propertyId<<" "<<latitude<<" "<<longitude<<" "<<price<<" "<<bedroom<<" "<<bathroom<<" "<<category<<"\n";
        }
};

class Query
{
    private:
        int id, latitude, longitude, distance, min_budget, max_budget, min_bedroom, max_bedroom, min_bathroom
        , max_bathroom;
    public:
        Query(int id, int latitude, int longitude,int distance, int min_budget, int max_budget, int min_bedroom,
              int max_bedroom, int min_bathroom, int max_bathroom)
              :id(id),latitude(latitude),longitude(longitude),distance(distance),min_budget(min_budget),max_budget(max_budget),
              min_bedroom(min_bedroom),max_bedroom(max_bedroom),min_bathroom(min_bathroom),max_bathroom(max_bathroom)
            {

            }
        void get_details()
        {
             cout<<id<<" "<<latitude<<" "<<longitude<<" "<<min_budget<<" "<<max_budget<<" "<<min_bedroom<<" "<<max_bedroom<<" "<<min_bathroom<<" "<<max_bathroom<<"\n";
        }

        vector<Property*> queryTree();
        void updateTree();
};
class Node
{
  public:
    int id, numberOfProperties, int latLeft,int lonLeft, int latRight, int lonRight;
    // number of places , if this value is greater than threshold we would split this node
    // each grid would be identified by the four edge coordinates
    vector < Property* > refArray; // would contain the addresses of all the objects of class Property so that we can fileter them
    // this array would be empty for non leaf nodes
    Node *children[4],parent;

    Node(int id,int latLeft,int lonLeft,int latRight, int lonRight)
      :id(id),latLeft(latLeft),lonLeft(lonLeft),latRight(latRight),lonRight(lonRight)
    {
      for(int i=0;i<4;i++){
        children[i] = NULL;
      }
      refArray.clear();
    }
    void populateRefArray()
    {
      // there should be a service which should return the address of the all objects of class Property so that we we can reference them
      // this array would be split as and when the number of properties on a node crosses a threshold
      // assume the service name is getPropertyReference
      for(int i=0;i<getPropertyReference.length();i++)
      {
        refArray.push_back(getPropertyReference[i]);
      }
    }

    // ideal design practice states that we should have the member variables and create getter and setter methids for same as below
    // but i am omitting them for the sake of brevity and making everything public
    // int get_id()
    // {
    //   return id;
    // }
}


void buildQuadTree(Node *currentNode)
{
    // assuming a limit of 500 places on a particular node, else i would break the
    if(currentNode->numberOfPlaces <= THRESHOLD)
    {
      return;
    }
    // now we need to split the node into 4 nodes, so first we need to find the boundary of each of the four grids
    int midLat = (currentNode->latLeft+currentNode->latRight)/2;
    int midLon = (currentNode->lonLeft+currentNode->lonRight)/2;
    int dtShift = ### // this small amount should be added so that quadants do not overlap which i am ignoring for cleanliness of code
    Node firstQuadrant(4*currentNode->id,currentNode->latLeft,currentNode->lonLeft,midLat,midLon);
    Node secondQuadrant(4*currentNode->id+1,currentNode->latLeft,midLon,midLat,currentNode->lonRight);
    Node thirdQuadrant(4*currentNode->id+2,midLat,currentNode->lonLeft,currentNode->latRight,midLon);
    Node fourthQuadrant(4*currentNode->id+3,midLat,midLon,currentNode->latRight,currentNode->lonRight);

    currentNode->children[0] = &firstQuadrant;
    currentNode->children[1] = &secondQuadrant;
    currentNode->children[2] = &thirdQuadrant;
    currentNode->children[3] = &fourthQuadrant;

    // now populate the refArray of all the 4 children as well as clear the parent as we stated that only leaves
    // contains the references
    for(int i=0;i<currentNode->refArray.size();i++)
    {
      Property eachProperty = *(currentNode->refArray[i]);
      if(eachProperty.latitude>=currentNode->latLeft && eachProperty.latitude<=midLat){
        if(eachProperty.longitude>=currentNode->lonLeft && eachProperty.longitude<=midLon){
          firstQuadrant.push_back(currentNode->refArray[i]);
        }
      }

      if(eachProperty.latitude>=currentNode->latLeft && eachProperty.latitude<=midLat){
        if(eachProperty.longitude>=midLon && eachProperty.longitude<=currentNode->lonRight){
          secondQuadrant.push_back(currentNode->refArray[i]);
        }
      }

      if(eachProperty.latitude>=midLat && eachProperty.latitude<=currentNode->latRight){
        if(eachProperty.longitude>=currentNode->lonLeft && eachProperty.longitude<=midLon){
          thirdQuadrant.push_back(currentNode->refArray[i]);
        }
      }

      if(eachProperty.latitude>=midLat && eachProperty.latitude<=currentNode->latRight){
        if(eachProperty.longitude>=midLon && eachProperty.longitude<=currentNode->lonRight){
          fourthQuadrant.push_back(currentNode->refArray[i]);
        }
      }
    }
    firstQuadrant.numberOfProperties = firstQuadrant.refArray.size();
    firstQuadrant.parent = currentNode;
    secondQuadrant.numberOfProperties = secondQuadrant.refArray.size();
    secondQuadrant.parent = currentNode;
    thirdQuadrant.numberOfProperties = thirdQuadrant.refArray.size();
    thirdQuadrant.parent = currentNode;
    fourthQuadrant.numberOfProperties = fourthQuadrant.refArray.size();
    fourthQuadrant.parent = currentNode;
    currentNode->refArray.clear(); // non leaf node
    // recursively repeat this procedure
    buildQuadTree(&firstQuadrant);
    buildQuadTree(&secondQuadrant);
    buildQuadTree(&thirdQuadrant);
    buildQuadTree(&fourthQuadrant);
}

bool isLeaf(Node* currentNode)
{
  bool ret = true;
  for(int i=0;i<4;i++)
  {
    if(currentNode->children[i]!=NULL){
      ret=false;
    }
  }
  return ret;
}

void merge(vector<Property*>a, vector<Property*> b){
  for(int i=0;i<b.size();i++)
  {
    a.push_back(b[i]);
  }
}

vector<Property*> Query::queryTree(Node *currentNode)
{
    // find the endpoints of the grid from the current location of agent
    vector<Property*> results;
    if(isLeaf(currentNode))
    {
      // check whether the endpoints of location fall in this range
      if(latitude>=currentNode->latLeft && latitude<=currentNode->latRight){
        if(longitude>=currentNode->lonLeft && longitude<=currentNode->lonRight){
          // this should be written taking into account the radius into consideraion which would involve
          // checking whether the current node overlaps the query grid
          // now assuming the grid overlaps ( i am assuming the grid converged to a point)
          // check for other cases like bedrooms etc
          for(int i=0;i<currentNode->numberOfPlaces;i++)
          {
            Property eachProperty = *(currentNode->refArray[i]);
            // the above checks are completely business logic stuff which can be customised
            if(price>=min_budget && price<=max_budget){
              if(bathroom>=min_bathroom && bathroom<=max_bathroom){
                if(bedroom>=min_bedroom && bedroom<=max_bedroom){
                  results.push_back(currentNode->refArray[i]);
                }
              }
            }
          }
        }
      }
      return;
    }
    vector<Property*>results1 = queryTree(currentNode->children[0]);
    vector<Property*>results2 = queryTree(currentNode->children[1]);
    vector<Property*>results3 = queryTree(currentNode->children[2]);
    vector<Property*>results4 = queryTree(currentNode->children[3]);
    // combine the results from all the children
    merge(&results,&results1);
    merge(&results,&results2);
    merge(&results,&results3);
    merge(&results,&results4);
    return results;
}

void updateTree(Node *currentNode)
{
  if(!isLeaf(currentNode))
  {
    // updating needs to happen at leaves nodes
    updateTree(currentNode->children[0]);
    updateTree(currentNode->children[1]);
    updateTree(currentNode->children[2]);
    updateTree(currentNode->children[3]);
    return;
  }
  if(latitude>=currentNode->latLeft && latitude<=currentNode->latRight){
    if(longitude>=currentNode->lonLeft && longitude<=currentNode->lonRight){
      // if the place falls in this grid
      if(currentNode->numberOfPlaces + 1 <= THRESHOLD){
        currentNode->numberOfPlaces+=1;
        currentNode->refArray.push_back(desiredProperty);
      } else {
        // we have to repeat the same procedure of splitting the node into 4 nodes as i did in the buildTree function
        // here i should have kept that part as a separate functionality which i didn't foresee
        // i could have done it but i hope you get the point
      }
    }
  }
}

int main()
{
    Node root(0,EXTREMELEFTLATITUDE,EXTREMELEFTLONGITUDE,EXTREMERIGHTLATITUDE,EXTREMERIGHTLONGITUDE,TOTALNUMBEROFPROPERTIES);
    root.populateRefArray(); // the root now contains the addresses of all instances of class Property
    root.numberOfProperties = root.refArray.size();
    root.parent=NULL;
    // a reference is used to save memory as well for the benefit that changes to teh property need to be done at one place , no need
    // to change at all places as we would simply print the value present at that address
    // building this tree is a one time job , for subsequent times we just need to update the tree
    buildQuadTree(root);

    // now the query starts
    Query q1(q_id,q_latitude,q_longitude,q_distance,q_min_budget,q_max_budget,q_min_bedroom,q_max_bedroom,q_min_bathroom,q_max_bathroom);
    q1.queryTree(root);

    // for updating
    q1.updateTree(root);
}
