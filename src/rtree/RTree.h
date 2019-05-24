/******************************************************************************
 * Project:  libspatialindex - A C++ library for spatial indexing
 * Author:   Marios Hadjieleftheriou, mhadji@gmail.com
 ******************************************************************************
 * Copyright (c) 2002, Marios Hadjieleftheriou
 *
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
******************************************************************************/

#pragma once

#include <memory>

#include "../spatialindex/SpatialIndex.h"
#include "../spatialindex/IdVisitor.h"
#include "Statistics.h"
#include "Node.h"
#include "PointerPoolNode.h"

namespace SpatialIndex
{
	namespace RTree
	{
			enum RTreeVariant
			{
					RV_LINEAR = 0x0,
					RV_QUADRATIC,
					RV_RSTAR
			};

			enum BulkLoadMethod
			{
					BLM_STR = 0x0
			};

			enum PersistenObjectIdentifier
			{
					PersistentIndex = 0x1,
					PersistentLeaf = 0x2
			};

			enum RangeQueryType
			{
					ContainmentQuery = 0x1,
					IntersectionQuery = 0x2
			};

		class RTree : public ISpatialIndex
		{
                  //class NNEntry;

		public:
			RTree(IStorageManager&, Tools::PropertySet&);
				// String                   Value     Description
				// ----------------------------------------------
				// IndexIndentifier         VT_LONG   If specified an existing index will be openened from the supplied
				//                          storage manager with the given index id. Behaviour is unspecified
				//                          if the index id or the storage manager are incorrect.
				// Dimension                VT_ULONG  Dimensionality of the data that will be inserted.
				// IndexCapacity            VT_ULONG  The index node capacity. Default is 100.
				// LeafCapactiy             VT_ULONG  The leaf node capacity. Default is 100.
				// FillFactor               VT_DOUBLE The fill factor. Default is 70%
				// TreeVariant              VT_LONG   Can be one of Linear, Quadratic or Rstar. Default is Rstar
				// NearMinimumOverlapFactor VT_ULONG  Default is 32.
				// SplitDistributionFactor  VT_DOUBLE Default is 0.4
				// ReinsertFactor           VT_DOUBLE Default is 0.3
				// EnsureTightMBRs          VT_BOOL   Default is true
				// IndexPoolCapacity        VT_LONG   Default is 100
				// LeafPoolCapacity         VT_LONG   Default is 100
				// RegionPoolCapacity       VT_LONG   Default is 1000
				// PointPoolCapacity        VT_LONG   Default is 500

			~RTree() override;



			//
			// ISpatialIndex interface
			//
			virtual void insertData(uint32_t len, const uint8_t* pData, const IShape& shape, id_type shapeIdentifier);
			virtual bool deleteData(const IShape& shape, id_type id);
			virtual void internalNodesQuery(const IShape& query, IVisitor& v);
			virtual void containsWhatQuery(const IShape& query, IVisitor& v);
			virtual void intersectsWithQuery(const IShape& query, IVisitor& v);
			virtual void pointLocationQuery(const Point& query, IVisitor& v);
			virtual void nearestNeighborQuery(uint32_t k, const IShape& query, IVisitor& v, INearestNeighborComparator&);
			virtual void nearestNeighborQuery(uint32_t k, const IShape& query, IVisitor& v);
			virtual void selfJoinQuery(const IShape& s, IVisitor& v);
			virtual void queryStrategy(IQueryStrategy& qs);
			virtual void getIndexProperties(Tools::PropertySet& out) const;
			virtual void addCommand(ICommand* pCommand, CommandType ct);
			virtual bool isIndexValid();
			virtual void getStatistics(IStatistics** out) const;
			virtual void flush();

		private:
			void initNew(Tools::PropertySet&);
			void initOld(Tools::PropertySet& ps);
			void storeHeader();
			void loadHeader();

			void insertData_impl(uint32_t dataLength, uint8_t* pData, Region& mbr, id_type id);
			void insertData_impl(uint32_t dataLength, uint8_t* pData, Region& mbr, id_type id, uint32_t level, uint8_t* overflowTable);
			bool deleteData_impl(const Region& mbr, id_type id);

			id_type writeNode(Node*);
			NodePtr readNode(id_type page);
			void deleteNode(Node*);

			void rangeQuery(RangeQueryType type, const IShape& query, IVisitor& v);
			void selfJoinQuery(id_type id1, id_type id2, const Region& r, IVisitor& vis);
			void visitSubTree(NodePtr subTree, IVisitor& v);

			IStorageManager* m_pStorageManager;

			id_type m_rootID, m_headerID;

			RTreeVariant m_treeVariant;

			double m_fillFactor;

			uint32_t m_indexCapacity;

			uint32_t m_leafCapacity;

			uint32_t m_nearMinimumOverlapFactor;
				// The R*-Tree 'p' constant, for calculating nearly minimum overlap cost.
				// [Beckmann, Kriegel, Schneider, Seeger 'The R*-tree: An efficient and Robust Access Method
				// for Points and Rectangles', Section 4.1]

			double m_splitDistributionFactor;
				// The R*-Tree 'm' constant, for calculating spliting distributions.
				// [Beckmann, Kriegel, Schneider, Seeger 'The R*-tree: An efficient and Robust Access Method
				// for Points and Rectangles', Section 4.2]

			double m_reinsertFactor;
				// The R*-Tree 'p' constant, for removing entries at reinserts.
				// [Beckmann, Kriegel, Schneider, Seeger 'The R*-tree: An efficient and Robust Access Method
				//  for Points and Rectangles', Section 4.3]

			uint32_t m_dimension;

			Region m_infiniteRegion;

			Statistics m_stats;

			bool m_bTightMBRs;

			Tools::PointerManager<Point> m_pointPool;
			Tools::PointerManager<Region> m_regionPool;
			Tools::PointerManager<Node> m_indexPool;
			Tools::PointerManager<Node> m_leafPool;

			std::vector<std::shared_ptr<ICommand> > m_writeNodeCommands;
			std::vector<std::shared_ptr<ICommand> > m_readNodeCommands;
			std::vector<std::shared_ptr<ICommand> > m_deleteNodeCommands;

			class NNEntry
			{
			public:
				id_type m_id;
				IEntry* m_pEntry;
				double m_minDist;

				NNEntry(id_type id, IEntry* e, double f) : m_id(id), m_pEntry(e), m_minDist(f) {}
				~NNEntry() = default;

				struct ascending : public std::binary_function<NNEntry*, NNEntry*, bool>
				{
					bool operator()(const NNEntry* __x, const NNEntry* __y) const { return __x->m_minDist > __y->m_minDist; }
				};
			}; // NNEntry

			class NNComparator : public INearestNeighborComparator
			{
			public:
				double getMinimumDistance(const IShape& query, const IShape& entry) override
				{
					return query.getMinimumDistance(entry);
				}

				double getMinimumDistance(const IShape& query, const IData& data) override
				{
					IShape* pS;
					data.getShape(&pS);
					double ret = query.getMinimumDistance(*pS);
					delete pS;
					return ret;
				}
			}; // NNComparator

			class ValidateEntry
			{
			public:
				ValidateEntry(Region& r, NodePtr& pNode) : m_parentMBR(r), m_pNode(pNode) {}

				Region m_parentMBR;
				NodePtr m_pNode;
			}; // ValidateEntry

			friend class Node;
			friend class Leaf;
			friend class Index;
			friend class BulkLoader;

			friend std::ostream& operator<<(std::ostream& os, const RTree& t);
		}; // RTree

		std::ostream& operator<<(std::ostream& os, const RTree& t);

			class Data : public IData, public Tools::ISerializable
			{
			public:
					Data(uint32_t len, uint8_t* pData, Region& r, id_type id);
					~Data() override;

					Data* clone() override;
					id_type getIdentifier() const override;
					void getShape(IShape** out) const override;
					void getData(uint32_t& len, uint8_t** data) const override;
					uint32_t getByteArraySize() override;
					void loadFromByteArray(const uint8_t* data) override;
					void storeToByteArray(uint8_t** data, uint32_t& len) override;

					id_type m_id;
					Region m_region;
					uint8_t* m_pData;
					uint32_t m_dataLength;
			}; // Data

			ISpatialIndex* returnRTree(IStorageManager& ind, Tools::PropertySet& in);
			ISpatialIndex* createNewRTree(
							IStorageManager& sm,
							double fillFactor,
							uint32_t indexCapacity,
							uint32_t leafCapacity,
							uint32_t dimension,
							RTreeVariant rv,
							id_type& indexIdentifier
			);
			ISpatialIndex* createAndBulkLoadNewRTree(
							BulkLoadMethod m,
							IDataStream& stream,
							IStorageManager& sm,
							double fillFactor,
							uint32_t indexCapacity,
							uint32_t leafCapacity,
							uint32_t dimension,
							RTreeVariant rv,
							id_type& indexIdentifier
			);
			ISpatialIndex* createAndBulkLoadNewRTree(
							BulkLoadMethod m,
							IDataStream& stream,
							IStorageManager& sm,
							Tools::PropertySet& ps,
							id_type& indexIdentifier
			);
			ISpatialIndex* loadRTree(IStorageManager& in, id_type indexIdentifier);
	}
	}
}
