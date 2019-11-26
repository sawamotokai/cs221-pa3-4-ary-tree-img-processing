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
  im = noFrame = imIn;
  int nodeSize = biggestPow2(min(imIn.width(), imIn.height()));
  numLeaf = 1;
  root = new Node(im, make_pair(0, 0), nodeSize, NULL);
  nodesQ.push(root);
  while (!nodesQ.empty())
  {
    Node *t = nodesQ.top();
    nodesQ.pop();
    split(t);
  }
}

QTree::QTree(PNG &imIn, int leafB, bool bal)
    : leafBound(leafB), balanced(bal), drawFrame(false)
{
  im = noFrame = imIn;
  int nodeSize = biggestPow2(min(imIn.width(), imIn.height()));
  numLeaf = 1;
  root = new Node(im, make_pair(0, 0), nodeSize, NULL);
  nodesQ.push(root);
  while (!nodesQ.empty())
  {
    Node *t = nodesQ.top();
    nodesQ.pop();
    split(t);
  }
}

bool QTree::isLeaf(Node *t)
{
  if (t == NULL)
    return false;
  return !(t->ne || t->nw || t->se || t->sw);
}

void QTree::splitHelper(Node *t)
{
  t->nw = new Node(im, make_pair(t->upLeft.first, t->upLeft.second), t->size / 2, t);
  nodesQ.push(t->nw);
  t->ne = new Node(im, make_pair(t->upLeft.first + t->size / 2, t->upLeft.second), t->size / 2, t);
  nodesQ.push(t->ne);
  t->sw = new Node(im, make_pair(t->upLeft.first, t->upLeft.second + t->size / 2), t->size / 2, t);
  nodesQ.push(t->sw);
  t->se = new Node(im, make_pair(t->upLeft.first + t->size / 2, t->upLeft.second + t->size / 2), t->size / 2, t);
  nodesQ.push(t->se);

  numLeaf += 3;

  if (balanced)
  {
    if (t->parent == NULL)
      return;
    if (t == t->parent->ne)
    {
      if (isLeaf(NNbr(t->parent)) && NNbr(t->parent)->size == t->parent->size)
        splitHelper(NNbr(t->parent));
      if (isLeaf(ENbr(t->parent)) && ENbr(t->parent)->size == t->parent->size)
        splitHelper(ENbr(t->parent));
    }
    else if (t == t->parent->se)
    {
      if (isLeaf(SNbr(t->parent)) && SNbr(t->parent)->size == t->parent->size)
        splitHelper(SNbr(t->parent));
      if (isLeaf(ENbr(t->parent)) && ENbr(t->parent)->size == t->parent->size)
        splitHelper(ENbr(t->parent));
    }
    else if (t == t->parent->sw)
    {
      if (isLeaf(SNbr(t->parent)) && SNbr(t->parent)->size == t->parent->size)
        splitHelper(SNbr(t->parent));
      if (isLeaf(WNbr(t->parent)) && WNbr(t->parent)->size == t->parent->size)
        splitHelper(WNbr(t->parent));
    }
    else if (t == t->parent->nw)
    {
      if (isLeaf(NNbr(t->parent)) && NNbr(t->parent)->size == t->parent->size)
        splitHelper(NNbr(t->parent));
      if (isLeaf(WNbr(t->parent)) && WNbr(t->parent)->size == t->parent->size)
        splitHelper(WNbr(t->parent));
    }
  }
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
}

/* NNbr(t)
 * return the same-sized quad tree node that is north of Node t.
 * return NULL if this node is not in the QTree.
 */
QTree::Node *QTree::NNbr(Node *t)
{
  if (t == NULL || t->parent == NULL)
    return NULL;
  // Node *ret = NULL;
  if (t == t->parent->ne)
  { // t is upper right of its parent
    if (NNbr(t->parent))
      return NNbr(t->parent)->se;
  }
  else if (t == t->parent->nw)
  { // upper left
    if (NNbr(t->parent))
      return NNbr(t->parent)->sw;
  }
  else if (t == t->parent->se)
    return t->parent->ne;
  else if (t == t->parent->sw)
    return t->parent->nw;
  return NULL;
  /* YOUR CODE HERE */
}

/* SNbr(t)
 * return the same-sized quad tree node that is south of Node t.
 * return NULL if this node is not in the QTree.
 */
QTree::Node *QTree::SNbr(Node *t)
{
  if (t == NULL || t->parent == NULL)
  {
    return NULL;
  }
  if (t == t->parent->ne) // t is upper right of its parent
    return t->parent->se;
  else if (t == t->parent->nw) // upper left
    return t->parent->sw;
  else if (t == t->parent->se)
  {
    if (SNbr(t->parent))
      return SNbr(t->parent)->ne;
  }
  else if (t == t->parent->sw)
  {
    if (SNbr(t->parent))
      return SNbr(t->parent)->nw;
  }
  return NULL;
  /* YOUR CODE HERE */
}

/* ENbr(t)
 * return the same-sized quad tree node that is east of Node t.
 * return NULL if this node is not in the QTree.
 */
QTree::Node *QTree::ENbr(Node *t)
{
  if (t == NULL || t->parent == NULL)
    return NULL;
  if (t == t->parent->ne) // t is upper right of its parent
  {
    if (ENbr(t->parent))
      return ENbr(t->parent)->nw;
  }
  else if (t == t->parent->nw) // upper left
    return t->parent->ne;
  else if (t == t->parent->se)
  {
    if (ENbr(t->parent))
      return ENbr(t->parent)->sw;
  }
  else if (t == t->parent->sw)
    return t->parent->se;
  return NULL;
  /* YOUR CODE HERE */
}

/* WNbr(t)
 * return the same-sized quad tree node that is west of Node t.
 * return NULL if this node is not in the QTree.
 */
QTree::Node *QTree::WNbr(Node *t)
{
  if (t == NULL || t->parent == NULL)
    return NULL;

  if (t == t->parent->ne) // t is upper right of its parent
    return t->parent->nw;
  else if (t == t->parent->nw)
  { // upper left
    if (WNbr(t->parent))
      return WNbr(t->parent)->ne;
  }
  else if (t == t->parent->se)
    return t->parent->sw;
  else if (t == t->parent->sw)
  {
    if (WNbr(t->parent))
      return WNbr(t->parent)->se;
  }
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
  }
  else
  {
    int xStart = node->upLeft.first;
    int yStart = node->upLeft.second;
    for (int x = xStart; x < xStart + node->size; x++)
    {
      for (int y = yStart; y < yStart + node->size; y++)
      {
        RGBAPixel *pix = im.getPixel(x, y);
        RGBAPixel *pixNoFrame = noFrame.getPixel(x, y);
        *pixNoFrame = node->avg;
        if (drawFrame && (x == xStart || y == yStart || x == xStart + node->size - 1 || y == yStart + node->size - 1))
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
  // delete (im);
  // delete(noFrame);
  // delete (frameColor);
  /* YOUR CODE HERE */
}

void QTree::clearHelper(Node *node)
{
  if (node == NULL)
    return;
  clearHelper(node->nw);
  clearHelper(node->ne);
  clearHelper(node->sw);
  clearHelper(node->se);
  delete node;
}

void QTree::copyHelper(Node *subRoot, Node *origNode)
{
  if (origNode == NULL || origNode->ne == NULL)
    return;
  subRoot->ne = new Node(noFrame, origNode->ne->upLeft, origNode->ne->size, subRoot);
  subRoot->nw = new Node(noFrame, origNode->nw->upLeft, origNode->nw->size, subRoot);
  subRoot->se = new Node(noFrame, origNode->se->upLeft, origNode->se->size, subRoot);
  subRoot->sw = new Node(noFrame, origNode->sw->upLeft, origNode->sw->size, subRoot);
  copyHelper(subRoot->ne, origNode->ne);
  copyHelper(subRoot->nw, origNode->nw);
  copyHelper(subRoot->se, origNode->se);
  copyHelper(subRoot->sw, origNode->sw);
}


void QTree::copy(const QTree &orig)
{
  this->numLeaf = orig.numLeaf;
  this->im = orig.im;
  this->leafBound = orig.leafBound;
  this->balanced = orig.balanced;
  this->drawFrame = orig.drawFrame;
  this->noFrame = orig.noFrame;
  this->frameColor = orig.frameColor;

  this->root = new Node(noFrame, orig.root->upLeft, orig.root->size, NULL);
  copyHelper(orig.root, root);
  /* YOUR CODE HERE */
}
