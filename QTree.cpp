/**
 *
 * Balanced Quad Tree (pa3)
 *
 * This file will be used for grading.
 *
 */

#include "QTree.h"

// Return the biggest power of 2 less than or equal to n
int biggestPow2(int n)
{
  if (n < 1)
    return 0;
  int v = 1;
  while (v <= n)
    v <<= 1;
  return v >> 1;
}

QTree::Node::Node(PNG &im, pair<int, int> ul, int sz, Node *par)
    : upLeft(ul), size(sz), parent(par), nw(NULL), ne(NULL), sw(NULL), se(NULL)
{
  var = varAndAvg(im, ul, size, avg);
}

// Defined by Kai
QTree::Node::Node(Node *other, Node* parent)
{
  if (other == NULL)
    return;
  this->upLeft = other->upLeft;
  this->size = other->size;
  this->parent = parent;
  this->nw = other->nw;
  this->ne = other->ne;
  this->sw = other->sw;
  this->se = other->se;
  this->var = other->var;
  this->avg = other->avg;
}

QTree::~QTree()
{
  clear();
}

QTree::QTree(const QTree &other)
{
  copy(other);
}

QTree &QTree::operator=(const QTree &rhs)
{
  if (this != &rhs)
  {
    clear();
    copy(rhs);
  }
  return *this;
}

QTree::QTree(PNG &imIn, int leafB, RGBAPixel frameC, bool bal)
    : leafBound(leafB), balanced(bal), drawFrame(true), frameColor(frameC)
{
  im = imIn;                
  int nodeSize = biggestPow2(imIn.width());
  numLeaf = 1;   
  root = new Node(im, make_pair(0, 0), nodeSize, NULL);
  nodesQ.push(root);
  while (!nodesQ.empty()) {
    Node* t = nodesQ.top(); 
    nodesQ.pop();
    split(t);
  }
}

QTree::QTree(PNG &imIn, int leafB, bool bal)
    : leafBound(leafB), balanced(bal), drawFrame(false)
{
  im = imIn;                
  int nodeSize = biggestPow2(imIn.width());
  numLeaf = 1;   
  root = new Node(im, make_pair(0, 0), nodeSize, NULL);
  nodesQ.push(root);
  while (!nodesQ.empty()) {
    Node* t = nodesQ.top(); 
    nodesQ.pop();
    split(t);
  }
}


bool QTree::isLeaf(Node *t)
{
  if (t==NULL)
    return false;
  return !(t->ne || t->nw || t->se || t->sw);
}

void QTree::splitHelper(Node *t) {
  t->ne = new Node(im, make_pair(t->upLeft.first+t->size/2, t->upLeft.second), t->size / 2, t);
  nodesQ.push(t->ne);
  t->se = new Node(im, make_pair(t->upLeft.first+t->size/2, t->upLeft.second+t->size/2), t->size / 2, t);
  nodesQ.push(t->se);
  t->sw = new Node(im, make_pair(t->upLeft.first, t->upLeft.second+t->size/2), t->size / 2, t);
  nodesQ.push(t->sw);
  t->nw = new Node(im, make_pair(t->upLeft.first, t->upLeft.second), t->size / 2, t);
  nodesQ.push(t->nw);
  
  numLeaf += 3;
}

void QTree::split(Node *t)
{
  /* YOUR CODE HERE */
  if (t == NULL || numLeaf >= leafBound || t->size <= 1 || !isLeaf(t))
    return;
  splitHelper(t);
  // FOR BALANCED QTREES-------------------------------------------------
  // A split might cause one or two nbrs of the parent of t to split
  // to maintain balance.  Note that these two nbrs exist (unless they're
  // not in the image region) because the current set of leaves are
  // balanced.
  // if( t is a NW (or NE or SW or SE) child ) then we need to check that
  // the North and West (or North and East or South and West or
  // South and East) nbrs of t->parent have children. If they don't
  // we need to split them.
  if (balanced) {
    if (t->parent==NULL) 
      return;
    if (t == t->parent->ne)
      {
        if (isLeaf(NNbr(t->parent)) && NNbr(t->parent)->size == t->parent->size)
          splitHelper(NNbr(t->parent));
        if (isLeaf(ENbr(t->parent)) && ENbr(t->parent)->size == t->parent->size)
          splitHelper(ENbr(t->parent));
    }
    else if (t==t->parent->se) {
      if (isLeaf(SNbr(t->parent)) && SNbr(t->parent)->size == t->parent->size)
        splitHelper(SNbr(t->parent));
      if (isLeaf(ENbr(t->parent)) && ENbr(t->parent)->size == t->parent->size)
        splitHelper(ENbr(t->parent));
    }
    else if (t==t->parent->sw) {
      if (isLeaf(SNbr(t->parent)) && SNbr(t->parent)->size == t->parent->size)
        splitHelper(SNbr(t->parent));
      if (isLeaf(WNbr(t->parent)) && WNbr(t->parent)->size == t->parent->size)
        splitHelper(WNbr(t->parent));
    }
    else if (t==t->parent->nw) {
      if (isLeaf(NNbr(t->parent)) && NNbr(t->parent)->size == t->parent->size)
        splitHelper(NNbr(t->parent));
      if (isLeaf(WNbr(t->parent)) && WNbr(t->parent)->size == t->parent->size)
        splitHelper(WNbr(t->parent));
    }
  }
}

/* NNbr(t)
 * return the same-sized quad tree node that is north of Node t.
 * return NULL if this node is not in the QTree.
 */
QTree::Node *QTree::NNbr(Node *t)
{
  if (t==NULL || t->parent==NULL)
    return NULL;
  Node *ret = NULL;
  if (t == t->parent->ne) {// t is upper right of its parent
    if (NNbr(t->parent))
      ret = NNbr(t->parent)->se;
  }
  else if (t == t->parent->nw) {// upper left
    if (NNbr(t->parent))
      ret = NNbr(t->parent)->sw;
  }
  else if (t == t->parent->se)
    ret = t->parent->ne;
  else if (t == t->parent->sw)
    ret = t->parent->nw;
  if (ret && isLeaf(ret))
    return ret;
  else
    return NULL;
  /* YOUR CODE HERE */
}

/* SNbr(t)
 * return the same-sized quad tree node that is south of Node t.
 * return NULL if this node is not in the QTree.
 */
QTree::Node *QTree::SNbr(Node *t)
{
  if (t==NULL || t->parent==NULL) {
    return NULL;
  }
  Node *ret = NULL;
  if (t == t->parent->ne) // t is upper right of its parent
    ret = t->parent->se;
  else if (t == t->parent->nw) // upper left
    ret = t->parent->sw;
  else if (t == t->parent->se) {
    if (SNbr(t->parent))
      ret = SNbr(t->parent)->ne;
  }
  else if (t == t->parent->sw) {
    if (SNbr(t->parent))
      ret = SNbr(t->parent)->nw;
  }
  if (ret && isLeaf(ret))
    return ret;
  else
    return NULL;
  /* YOUR CODE HERE */
}

/* ENbr(t)
 * return the same-sized quad tree node that is east of Node t.
 * return NULL if this node is not in the QTree.
 */
QTree::Node *QTree::ENbr(Node *t)
{
  if (t==NULL || t->parent==NULL)
    return NULL;
  Node *ret = NULL;
  if (t == t->parent->ne) // t is upper right of its parent
  {
    if (ENbr(t->parent))
      ret = ENbr(t->parent)->nw;
  }
  else if (t == t->parent->nw) // upper left
    ret = t->parent->ne;
  else if (t == t->parent->se)
    {
      if (ENbr(t->parent))
      ret = ENbr(t->parent)->sw;
    }
  else if (t == t->parent->sw)
    ret = t->parent->se;
  if (ret && isLeaf(ret))
    return ret;
  else
    return NULL;
  /* YOUR CODE HERE */
}

/* WNbr(t)
 * return the same-sized quad tree node that is west of Node t.
 * return NULL if this node is not in the QTree.
 */
QTree::Node *QTree::WNbr(Node *t)
{
  if (t==NULL || t->parent==NULL)
    return NULL;
  Node *ret = NULL;
  if (t == t->parent->ne) // t is upper right of its parent
    ret = t->parent->nw;
  else if (t == t->parent->nw) {// upper left
    if (WNbr(t->parent))
      ret = WNbr(t->parent)->ne;
  }
  else if (t == t->parent->se)
    ret = t->parent->sw;
  else if (t == t->parent->sw) {
    if (WNbr(t->parent))
      ret = WNbr(t->parent)->se;
  }
  if (ret && isLeaf(ret))
    return ret;
  else
    return NULL;
  /* YOUR CODE HERE */
}

bool QTree::write(string const &fileName)
{
  /* YOUR CODE HERE */
  writeHelper(root);
  // include the following line to write the image to file.
  return (im.writeToFile(fileName));
}

void QTree::writeHelper(Node *node)
{
  if (!node)
    return;
  if (!isLeaf(node))
  {
    writeHelper(node->ne);
    writeHelper(node->se);
    writeHelper(node->nw);
    writeHelper(node->sw);
  } else {
    int xStart = node->upLeft.first;
    int yStart = node->upLeft.second;
    for (int x = xStart; x < xStart + node->size; x++)
    {
      for (int y = yStart; y < yStart + node->size; y++)
      {
        RGBAPixel *pix = im.getPixel(x, y);
        if (drawFrame && (x == xStart || y == yStart || x == xStart + node->size-1 || y == yStart + node->size-1))
          *pix = frameColor;
        else
          *pix = node->avg;
      }
    }
  }
}

void QTree::clear()
{
  clearHelper(root);
  // delete this;
  /* YOUR CODE HERE */
}

void QTree::clearHelper(Node *node) {
  if (node==NULL)
    return;
  clearHelper(node->ne);
  clearHelper(node->nw);
  clearHelper(node->sw);
  clearHelper(node->se);
  delete (node);
}

void QTree::copyHelper(Node *subRoot, Node* origNode)
{
  if (origNode==NULL || origNode->ne==NULL)
    return;
  subRoot->ne = new Node(im, origNode->ne->upLeft, origNode->ne->size, subRoot);
  // subRoot->ne = new Node(origNode->ne, subRoot);
  subRoot->nw = new Node(im, origNode->nw->upLeft, origNode->nw->size, subRoot);
  // subRoot->nw = new Node(origNode->nw, subRoot);
  // subRoot->se = new Node(origNode->se, subRoot);
  subRoot->se = new Node(im, origNode->se->upLeft, origNode->se->size, subRoot);
  // subRoot->sw = new Node(origNode->sw, subRoot);
  subRoot->sw = new Node(im, origNode->sw->upLeft, origNode->sw->size, subRoot);
  copyHelper(root->ne, origNode->ne);
  copyHelper(root->nw, origNode->nw);
  copyHelper(root->se, origNode->se);
  copyHelper(root->sw, origNode->sw);
}

void QTree::copy(const QTree &orig)
{
  // root = new Node(orig.root, NULL);
  numLeaf = orig.numLeaf;
  im = orig.im;
  leafBound = orig.leafBound;
  balanced = orig.balanced;
  drawFrame = orig.drawFrame;
  frameColor = orig.frameColor;
  root = new Node(im, orig.root->upLeft, orig.root->size, NULL);

  copyHelper(root, orig.root);
  /* YOUR CODE HERE */
}