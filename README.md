# Designing-Scalable-Search

# Requirements and Goals of the system

Our service will be storing information about different properties (sale and rental) so that buyer and renter can perform a search on them based on a particular set of criterias. Upon querying , our service will return a list of properties around the user

# Functional Requirements:
 Agents should be able to add/delete/update properties.
Given their location , they should be able to find all the nearby properties which meet their search criteria.

# Non-Functional Requirements:
Users should have real-time search experience with minimum latency.
Our service should support a heavy search load. There will be lot of search requests compared to adding or updating properties.( One of the assumptions i am making in the design).

# Database Schema:
PropertyId (8 bytes) Uniquely identifies a property.
Latitude (8 bytes)
Longitude (8 bytes)
Price (4 bytes)
Bedrooms (1 byte)
Bathrooms(1 byte)
Category(1 byte) (Sale or rental)

With future growth in mind i am having 8 bytes for the PropertyId.
We can have a separate store to store photos or reviews of the property.

# System APIs

We can have a REST api to expose the functionality of our service.

search(api_dev_key, search_terms, user_location, radius_filter, maximum_results_to_return,                                           category_filter, sort, page_token)

Parameters:
api_dev_key(string) : The API developer key of the account to throttle users based on their    allocated quota.
seach_terms(object): This will contain the search terms as in the criteria.
user_location(string): Location of the user performing the search.
radius_filter(number): search radius in metres( assumption: I am stressing on this criteria as once the results are determined based on radius,  remaining parameters would be a simple filtering on them)
maximum_results_to_return(number): Number of results to return.
category_filter(string): Optional , to filter based on sale or rental.
sort(number): default best matched, can be changed to minimum distance etc
page_token(string): This will specify a page in the result set that should be returned.

Return 
A json containing information about the items matched with it’s desired properties.

# System Design and Algorithm:
We need to store and index each dataset of the properties. For users to query the massive database which is mainly read only(assumption), the indexing should be read efficient to return the results in near real time.
Given the location and attributes of a property doesn’t change that often , we don’t need to worry about frequent updates of the data.

As in the subsequent steps i would be first be focusing to get results within a particular radius and then would filter them based on the other criterias like budget,rooms etc as i think the first one is a bit difficult(my assumption)

Approaches to find matches within a given radius 
SQL solution:
One simple solution would be to store all the data in a database like MySql . Each property will be stored in a separate row , uniquely identified by its propertyID. Each property will have its latitude and longitude stored in different columns and to perform a fast search on them we should have indexes on both these fields.
To find all the nearby properties of a given location (X,Y) within a radius ‘D’ , we can query like this:
Select * from Properties where Latitude between X-D and X+D and Longitude between Y-D and Y+D

This query is not adequate exactly as to find distance between places we have to use pythagorean theorem but it would suffice for this basic solution.
Also this query would be quite inefficient , since we have two separate indexes and each index can return a huge list of places and performing on those lists won’t be efficient. I we can somehow shorten these lists, we can improve the performance of our query.

      2.   Grids:
We can divide the whole map into smaller grids to group locations into smaller sets. Each grid will store properties residing within a specific range of latitude and longitude. Based on a given location and radius , we can find all the neighbouring grids and then query these grids to find the nearby places.Grid size would be an important metric now and for the time being we can assume that it is equal to the distance we want to query. Since grid size is now statically defined , we can easily find the grid number of any location(lat,lon) and its neighbouring grids.If the radius being queried isn’t equal to the grid size we would need to merge grids and do the query.
In the database we can store gridId with each location and have an index on it too for faster searching.

Now the query would be like
Select * from Properties where Latitude between X-D and X+D and Longitude between Y-D and Y+D and GridID in (GridId, GridId1,..., GridId8)

This will undoubtedly improve the runtime of our query. 
We can also keep an in memory index in a hash table where the key would be grid number and value would be list of places contained in it for fast performance.
But the problem is that this query will still run slow for grids that have lot of properties in them , areas which are densely populated.
This can be solved if we can break a grid in 4 smaller grids when the no of properties that are contained in this grid is large.This means that densely populated areas will have a large number of grids while sparsely populated areas will have very few.

We can use a Quad Tree for this, wherein each node will represent a grid and it will have four children when we partition a grid into 4 smaller grids.Leaf nodes will represent the grids that cannot be further broken down. So leaf nodes will contain a list of properties with them. The root node will represent the entire world and we would break it down until we obtain the complete tree. To find the grid for the given location we will start at the root and move to the desired grid until we land at one of the leaves which will contain our list of properties.

To find the neighbouring grids of a given grid we can have a doubly linked list at the leaves which would make traversal to nearby nodes(hence list of properties easier) or we can follow the back link to the parent and move to the desired child nodes.
This is the time when we apply the complete the remaining parameters search and return the results as expected.

How to trace the desired leaf is similar to what a segment tree does in 2 dimension.( I am ignoring the explanation for the quad tree) as it’s just an extension to it.

What will be the search workflow?
We will first find the node that contains the user’s location. If that node has enough properties that meet our search criteria we can return to the client , else we can repeat the same for the neighbouring node using the doubly linked list until we find the required number of properties or we have exhausted the search radius.

Inserting a new property is similar , we have to update the db as well as the quad tree. If our quad tree resides in a single server adding a new property is very easy, else if it is distributed we have to first find the server that holds the quad tree of that location and insert it.


This is the best i can think of and i believe that this solution can support a large no of read requests keeping the assumptions i had mentioned in mind.
I am a bit confused about how to use the numbers (like 40% match ) exactly as the question was quite open ended. If you give me sufficient info about how that is to be incorporated i would further do it. As far as i think if i get the properties according to the search criteria in a given radius the remaining percentage match is just a set of if / else statements .

For updates which would be very less we can have a chron job to update the tree.

Please do revert back if you want any additional changes to be performed.

