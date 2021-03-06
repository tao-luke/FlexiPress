#ifndef ___HUFF
#define ___HUFF
#include "transform.h"
#include "string"

/**
 * Node serves as the parent class of InsideNode and LeafNode and together represents a huffman trie.
 * - members: 
 *  INHERITED:
 *     - m_next: a transform * pointing to the next Transformation.
 *     - m_encode: a boolean used to indicate if the transformation is to decode or encode
 *     - m_encodeMap: a mapping of (encoded value, encoded value length in bits)->actual char
 *                    #reasoning: we have at most 256 chars to be encoded, thus the length of the encoded val
 *                    # in huffman is no greater than 255. but the actual number represented by this 255length bit
 *                    much greater than a char, so we store both the encoded val and length of encoding
 *     - m_original_size:  the unsigned int count of the original file(also accurate in byte)
 *     - m_end_valid_bits: an unsigned char representing the number of bits in the last byte that are valid.(used in decoding)
 * 	LOCAL:
 * 	...to be updated
 */
class Node{//superclass node 
	pair<Node *, int> parent;  //each node has a parent and a "link" from the parent to the child, so the pair.second would be the link
	long freq;

public:
	Node(pair<Node *, int> parent,int freq):parent(parent),freq(freq){
	}
	Node* getParent(){
		return parent.first;
	}
	int getPLink(){
		return parent.second;
	}
	void setParent(pair<Node *, int> xparent){
		parent = xparent;
	}
	long getFreq(){
		return freq;
	}
	void upFreq(long f){ //freq++
		freq += f;
	}
	virtual int getChar() = 0; //mark this class as pure virtual //int so we can use -1 as a marking
	virtual ~Node(){};
};

class InsideNode: public Node{ //one possible node is insidenode 
	vector<Node *> next{}; //inside node has childrens
	public:
	InsideNode(int d,Node *parent = nullptr):Node(make_pair(parent,-1),0){
		next.reserve(d); 
		for (int i = 0; i < d; i++){ //fill with nullptr
			next.push_back(nullptr);
		}
	};
	void addChild(int c,Node* child){
		next[c] = child;
		upFreq(child->getFreq());
	}
	~InsideNode(){
		for(auto & n:next){
			delete n;
		}
	}
	int getChar() override{ //invalid char get case
		return -1;
	}
	vector<Node *> &getNext(){
		return next;
	}
};
class LeafNode: public Node{  //where the actual chars are stored
	unsigned char c;
	public:
	LeafNode( unsigned char c,long freq,Node* parent = nullptr): Node(make_pair(parent,0),freq),c(c){
	}
	int getChar() override{
		return c;
	}
	~LeafNode(){}
};

class Huff: public Transform
{
    vector<Node*> minHeap{}; //the min heap
	vector<long> freqMap{}; //mapping of frequency when first run through input
	vector<LeafNode *> charMap{}; //link the leafnodes to a vector for ease of access

    int size = 0;
    void heapify();
	long leftChild(long i);
	long rightChild(long i);
	long parent(long i);
	void fixDown(long i, long cap);
	void fixUp(long i);
	Node *getMin();
	void pop();


    // string getEncode(int c); //return the upward traversal from a leaf
    pair<long,unsigned char> getEncode(unsigned char n);
    void transform(vector<unique_ptr<Block> > &input) override;
    void applyTo(vector<long>& data) override;
    void decode(vector<unique_ptr<Block> > &input) override;
    void deplyTo(vector<long> &data);
    unsigned char decodeChar(pair<long, unsigned char> input);

public:
    Huff(Transform *next = nullptr); //! there can not be a next after this one. we serialize after this
    ~Huff(){
		if (!minHeap.empty())
			delete minHeap[0];
	}
};

#endif