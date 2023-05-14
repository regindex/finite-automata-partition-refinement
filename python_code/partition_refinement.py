#!/usr/bin/env python3

import networkx as nx
import matplotlib.pyplot as plt

def construct_graph(labeled_edge_list, s):
  G = nx.DiGraph()
  Sigma = set()
  for u, v, l in labeled_edge_list:
    G.add_edge(u, v, letter=l)
    Sigma.add(l)
  G.graph['alphabet'] = sorted(list(Sigma))
  G.graph['source'] = s
  return G


def draw(G):
  pos = nx.spring_layout(G)
  nx.draw_networkx(G, pos)
  edge_labels = dict([((n1, n2), d['letter'])
                      for n1, n2, d in G.edges(data=True)])
  nx.draw_networkx_edge_labels(G, pos, edge_labels=edge_labels)
  plt.show()


def input_partition(G, reverse=False):
  Sigma = G.graph['alphabet']
  P = [set([])] + [set([]) for s in Sigma]
  pos_s = {Sigma[i]: i + 1 for i in range(len(Sigma))}
  
  for v in G.nodes():
    in_letters = set()
    for _, _, d in G.in_edges(nbunch=v, data=True):
      in_letters.add(d['letter'])
    if len(in_letters) > 1:
      print('Node', v, 'has in-edges with letters:', in_letters)
      assert(False)
    elif len(in_letters) == 0:
      if v == G.graph['source']:
        P[0].add(v)
      else:
        print('Node', v, 'has no in-edges but is not the source.')
        assert(False)
    else:
      l = list(in_letters)[0]
      P[pos_s[l]].add(v)
      G.nodes[v]['in_letter'] = l
  if reverse:
    P.reverse()
  return P


def first_or_last_block_contained(S, P, first_or_last=False):
  print("S", S)
  print("P", P)
  for i in range(len(P)):
    if P[i] <= S:
      break
  for j in range(i + 1, len(P)):
    if not P[j] <= S:
      break
  if P[len(P) - 1] <= S:
    j = len(P)
  print("i", i, "j", j)
  if len(P[i]) <= len(P[j-1]) or not first_or_last:
    return True, P[i]
  else:
    return False, P[j-1]


def update_X(X, B, S, S_wo_B, first):
  for i in range(len(X)):
    if X[i] == S:
      if first:
        X[i] = B
        X.insert(i + 1, S_wo_B)
      else:
        X[i] = S_wo_B
        X.insert(i + 1, B)


def update_C(C, P, S_wo_B):
  for i in range(len(P)):
    if P[i] < S_wo_B:
      C.insert(0, S_wo_B)
      break


def reachable(G, R, letter):
  # print("R", R, "letter", letter)
  return set([v for _, v, d in G.out_edges(nbunch=R, data=True) if d['letter'] == letter])


def update_P(P, D, D11, D12, D2, first):
    non_empty_count = 0
    for i in range(len(P)):
      if P[i] == D:
        if first:
          if len(D2) != 0:
            P.insert(i, D2)
            non_empty_count += 1
          if len(D11) != 0:
            P.insert(i, D11)
            non_empty_count += 1
          if len(D12) != 0:
            P.insert(i, D12)
            non_empty_count += 1
          P.pop(i + non_empty_count)
          break
        else:
          if len(D12) != 0:
            P.insert(i, D12)
            non_empty_count += 1
          if len(D11) != 0:
            P.insert(i, D11)
            non_empty_count += 1
          if len(D2) != 0:
            P.insert(i, D2)
            non_empty_count += 1
          P.pop(i + non_empty_count)
          break
    assert non_empty_count >= 1
    return non_empty_count


def update_C_2(C, X, D, non_empty_count):
  if non_empty_count == 1:
    return 
  else:
    if len(C) == 0:
      C.append(D)
    old_i = 0
    for j in range(len(C)):
      for i in range(old_i, len(X)):
        if X[i] == C[j]: 
          if D <= X[i]:
            return
          else:
            break
        if D <= X[i]:
          C.insert(j, X[i])
          return 
      old_i = i + 1
    print("This should not happen.")
    assert(False)

def prune_edges(G, B, RB, RS_wo_B, first):

  if first == False:
    for node in set(RS_wo_B):
      if node in set(RB):
        inlist = list(G.in_edges(node, data=False))
        RB.remove(node)
        for j in range(len(inlist) ):
          if inlist[j][0] in B:
            print("remove edge",(inlist[j][0], node))
            G.remove_edge(inlist[j][0], node)
  else:
    for node in set(RB):
      if node in set(RS_wo_B):
        inlist = list(G.in_edges(node, data=False))
        RS_wo_B.remove(node)
        for j in range(len(inlist) ):
          if inlist[j][0] not in B:
            print("remove edge",(inlist[j][0], node))
            G.remove_edge(inlist[j][0], node)
  
  return RB, RS_wo_B
    

def ordered_paige_tarjan_edges_deletion(G, P):
  """Computes an ordered partition on the pruned graph.

    Keyword arguments:
    G -- the labeled input-consistent graph
    P -- the input partition
    """
  X = [set(G.nodes())]
  C = [set(G.nodes())]

  # until C is empty
  while len(C) > 0:

    print("P", P)
    print("X", X)
    print("C", C)
    # select current S
    S = C.pop(0)
    # select the splitter B
    first, B = first_or_last_block_contained(S, P)
    # compute S without B
    S_wo_B = S - B

    print('B', B, "S \\B", S_wo_B)

    update_X(X, B, S, S_wo_B, first)
    update_C(C, P, S_wo_B)

    Bp = B
    i = 0
    print("P = ",str(P))
    while i < len(P):
      # select D
      D = P[i]
      print("D = ", D)

      # if D is the block consisting of only the source, do nothing
      if set([G.graph['source']]) == D:
        print("+++++++++++++++++++++++++")
        print("end D iteration")
        print("+++++++++++++++++++++++++")
        i += 1
        continue
      # select edge label
      in_letter = G.nodes[list(D)[0]]['in_letter']
      # compute nodes that can be reached from B
      RB = reachable(G, Bp, in_letter)
      print("RB", RB)
      # compute nodes that can be reached from B/S
      RS_wo_B = reachable(G, S_wo_B, in_letter)
      print("RS_wo_B", RS_wo_B)
      # check if we can prune an edge
      RB, RS_wo_B = prune_edges(G, Bp, RB, RS_wo_B, first)
      # compute the splits
      D1 = D & RB
      D2 = D - D1 
      D11 = D1 & RS_wo_B
      D12 = D1 - D11 

      print("D1", D1)
      print("D2", D2)
      print("D11", D11)
      print("D12", D12)

      non_empty_count = update_P(P, D, D11, D12, D2, first)
      i += non_empty_count
      update_C_2(C, X, D, non_empty_count)
      
      print("P", P)
      print("X", X)
      print("C", C)

      print("+++++++++++++++++++++++++")
      print("end D iteration")
      print("+++++++++++++++++++++++++")

    print("==================================================")
    print("end B iteration")
    print("==================================================")

  return P

def ordered_paige_tarjan(G, P, first_or_last=True):
  """Computes an ordered partition.

    Keyword arguments:
    G -- the labeled input-consistent graph
    P -- the input partition
    """
  X = [set(G.nodes())]
  C = [set(G.nodes())]

  # cnt = 0

  while len(C) > 0:
    # cnt += 1
    # if cnt > 10:
    #   assert False

    print("P", P)
    print("X", X)
    print("C", C)

    S = C.pop(0)
    first, B = first_or_last_block_contained(S, P, first_or_last=first_or_last)
    S_wo_B = S - B

    print('B', B, "S\\B", S_wo_B)

    update_X(X, B, S, S_wo_B, first)
    update_C(C, P, S_wo_B)

    min_in_edge = {}

    Bp = B 

    i = 0
    while i < len(P):
      D = P[i]

      print("D", D)

      # if D is the block consisting of only the source, do nothing
      if set([G.graph['source']]) == D:
        print("+++++++++++++++++++++++++")
        print("end D iteration")
        print("+++++++++++++++++++++++++")
        i += 1
        continue
      
      in_letter = G.nodes[list(D)[0]]['in_letter']
      print("in_letter", in_letter)
      
      RB = reachable(G, Bp, in_letter)
      D1 = D & RB
      D2 = D - D1

      RS_wo_B = reachable(G, S_wo_B, in_letter)
      D11 = D1 & RS_wo_B
      D12 = D1 - D11

      print("D1", D1)
      print("D2", D2)
      print("D11", D11)
      print("D12", D12)

      non_empty_count = update_P(P, D, D11, D12, D2, first)
      i += non_empty_count
      update_C_2(C, X, D, non_empty_count)
      
      print("P", P)
      print("X", X)
      print("C", C)

      print("+++++++++++++++++++++++++")
      print("end D iteration")
      print("+++++++++++++++++++++++++")

      # if i >= 10:
      #   return

    print("==================================================")
    print("end B iteration")
    print("==================================================")

  return P

def main():

  import random as rm

  s = 1
  labeled_edge_list = [(1, 7, 'b'), (7, 2, 'a'), (2, 3, 'a'), (3, 3, 'a'), (3, 4, 'b'), (1, 8, 'a'), (8, 5, 'b'), (5, 6, 'a'), (6, 6, 'a'), (6, 4, 'b')]

  G = construct_graph(labeled_edge_list, s)
  P = input_partition(G, reverse=False)
  P = ordered_paige_tarjan_edges_deletion(G, P)
  print(P)

  labeled_edge_list = [(0, 1, 'c'), (1, 2, 'a'), (2, 3, 'a'), (3, 4, 'a'), (0, 5, 'b'), (5, 1, 'c'), (5, 4, 'a')]

  G = construct_graph(labeled_edge_list, 0)
  P = input_partition(G, reverse=False)
  P = ordered_paige_tarjan_edges_deletion(G, P)
  print(P)

